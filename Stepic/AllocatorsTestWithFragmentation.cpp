/*
 * AllocatorsTest.cpp
 *
 *  Created on: Jun 1, 2016
 *      Author: denis
 */

#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <algorithm>

namespace
{
	class SmallFragmentationAllocator
	{
			std::map<size_t, size_t> findFreeBlocks()
			{
				std::map<size_t, size_t> freeBlocks;
				size_t begin = 0;
				for (auto block : occupiedMemory)
				{
					if (block.first - begin > 0)
						freeBlocks.insert(std::make_pair(block.first - begin, begin));
					begin = block.first + block.second;
				}
				if (memorySize - begin > 0)
						freeBlocks.insert(std::make_pair(memorySize - begin, begin));
				return freeBlocks;
			}

			void* AllocRaw(unsigned int Size)
			{
				if (Size > memorySize)
					throw std::bad_alloc();

				if (occupiedMemory.empty())
				{
					occupiedMemory.insert(std::make_pair(0, Size));
					return Memory;
				}

				std::map<size_t, size_t> freeMemory = findFreeBlocks();

				auto it = freeMemory.upper_bound(Size);
				if (it == freeMemory.end())
					throw std::bad_alloc();

				occupiedMemory.insert(std::make_pair(it->second, Size));
				return Memory + it->second;
			}

			void FreeRaw(void* Pointer)
			{
				auto it = occupiedMemory.find((char*)Pointer - Memory);
				if (it != occupiedMemory.end())
				{
					memset(Pointer, 0, it->second);
					occupiedMemory.erase(it);
				}
			}

	public:
			SmallFragmentationAllocator()
			{
				memset(Memory, 0, sizeof(Memory));
			}

			class pointer
			{
				void destroy()
				{
					allocator->pointers.erase(id);
					destroyed = true;
				}
			public:
				template<class T>
				T* operator->()
				{
					auto it = allocator->pointers.find(id);
					if (it != allocator->pointers.end())
						return (T*)it->second;
					throw std::runtime_error("bad pointer");
				}
				template<class T>
				operator T*()
				{
					auto it = allocator->pointers.find(id);
					if (it != allocator->pointers.end())
						return (T*)it->second;
					throw std::runtime_error("bad pointer");
				}

				~pointer()
				{
					if (!destroyed)
						destroy();
				}
				pointer(const pointer& Pointer) = delete;
				pointer& operator=(const pointer& Pointer) = delete;
				pointer& operator=(pointer&& Pointer) = delete;

				pointer(pointer&& Pointer) : id(staticId), allocator(Pointer.allocator)
				{
					allocator->pointers.insert(std::make_pair(id, (void*)Pointer));
					staticId++;
					if (!Pointer.destroyed)
						Pointer.destroy();
				}

			private:
				pointer(SmallFragmentationAllocator *allocator, void* ptr) : id(staticId), allocator(allocator)
				{
					allocator->pointers.insert(std::make_pair(id, ptr));
					staticId++;
				}

				bool destroyed{false};
				static size_t staticId;
				const size_t id; // should not simply increment but take first available
				SmallFragmentationAllocator* allocator;
				friend class SmallFragmentationAllocator;
			};

			friend class pointer;

			static const int memorySize = 1024;
			//static const int memorySize = 1048576;

			pointer Alloc(unsigned int Size)
			{
				return {this, AllocRaw(Size)};
			}

			void ReAlloc(pointer& Pointer, unsigned int Size)
			{
				if (Pointer.destroyed)
					return;

				auto it = occupiedMemory.find((char*)Pointer - Memory);
				if (it == occupiedMemory.end())
					return;

				auto beginOfNextBlock = it;
				++beginOfNextBlock;

				size_t offsetNextBlock = 0;
				if (beginOfNextBlock != occupiedMemory.end())
					offsetNextBlock = beginOfNextBlock->first;
				else
					offsetNextBlock = memorySize;

				if (it->first + Size < offsetNextBlock)
				{
					// no reallocation
					it->second = Size;
					pointers[Pointer.id] = Memory + it->first;
					return;
				}

				auto retPtr = AllocRaw(Size);
				memcpy(retPtr, Pointer, it->second);
				FreeRaw(Pointer);
				pointers[Pointer.id] = retPtr;
			}

			void Free(pointer& Pointer)
			{
				if (Pointer.destroyed)
					return;

				FreeRaw((void*)Pointer);
				Pointer.destroy();
			}

			void defragment()
			{
				std::map<size_t, size_t> idLinks;
				for (auto& elem : pointers)
				{
					idLinks.insert(std::make_pair((char*)elem.second - Memory, elem.first));
				}
				size_t currentOffset = 0;
				while (currentOffset < memorySize)
				{
					auto it = occupiedMemory.lower_bound(currentOffset);
					if (it == occupiedMemory.end())
						break;

					if (it->first > currentOffset)
					{
						// move the memory to new location
						std::rotate(Memory + currentOffset, Memory + it->first, Memory + it->first + it->second);

						// update smart pointers
						auto id = idLinks[it->first];
						pointers[id] = Memory + currentOffset;

						// remember block offset
						auto blockOffset = it->first;

						// update occupiedMap
						occupiedMemory.insert(std::make_pair(currentOffset, it->second));
						occupiedMemory.erase(it);

						// jump to the end of the old block
						currentOffset += blockOffset;
					}
					else
					{
						// this block is on place, just skip it;
						currentOffset += it->second;
					}
				}
			}
	private:
			std::map<size_t, size_t> occupiedMemory;
			std::map<size_t, void*> pointers;
			char Memory[memorySize];
	};
	size_t SmallFragmentationAllocator::pointer::staticId = 0;
}

TEST(SmallFragmentationAllocatorUnitTest, simple_test)
{
	SmallFragmentationAllocator A1;
	auto A1_P1 = A1.Alloc(sizeof(int));
	A1.ReAlloc(A1_P1, 2 * sizeof(int));
	A1.Free(A1_P1);
}

TEST(SmallFragmentationAllocatorUnitTest, main_test)
{
	SmallFragmentationAllocator A2;
	auto A2_P1 = A2.Alloc(10 * sizeof(int));
	for(int i = 0; i < 10; i++) ((int*)A2_P1)[i] = i;
	for(int i = 0; i < 10; i++) if(((int*)A2_P1)[i] != i) std::cout << "ERROR 1" << std::endl;

	auto A2_P2 = A2.Alloc(10 * sizeof(int));
	for(int i = 0; i < 10; i++) ((int*)A2_P2)[i] = -1;
	for(int i = 0; i < 10; i++) if(((int*)A2_P1)[i] != i) std::cout << "ERROR 2" << std::endl;
	for(int i = 0; i < 10; i++) if(((int*)A2_P2)[i] != -1) std::cout << "ERROR 3" << std::endl;
	A2.ReAlloc(A2_P1, 20 * sizeof(int));
	for(int i = 10; i < 20; i++) ((int*)A2_P1)[i] = i;
	for(int i = 0; i < 20; i++) if(((int*)A2_P1)[i] != i) std::cout << "ERROR 4" << std::endl;
	for(int i = 0; i < 10; i++) if(((int*)A2_P2)[i] != -1) std::cout << "ERROR 5" << std::endl;
	A2.ReAlloc(A2_P1, 5 * sizeof(int));
	for(int i = 0; i < 5; i++) if(((int*)A2_P1)[i] != i) std::cout << "ERROR 6" << std::endl;
	for(int i = 0; i < 10; i++) if(((int*)A2_P2)[i] != -1) std::cout << "ERROR 7" << std::endl;
	A2.Free(A2_P1);
	A2.Free(A2_P2);
}

TEST(SmallFragmentationAllocatorUnitTest, failing_cases)
{
	SmallFragmentationAllocator A2;
	auto A2_P1 = A2.Alloc(10 * sizeof(int));

	A2.Free(A2_P1);

	EXPECT_ANY_THROW(((int*)A2_P1));

	// not compiles
	// A2_P1 = A2.Alloc(10 * sizeof(int));

	// no effect
	A2.Free(A2_P1);
	A2.ReAlloc(A2_P1, 10 * sizeof(int));
	// after reallocation pointer is still not valid
	EXPECT_ANY_THROW(((int*)A2_P1));
}

TEST(SmallFragmentationAllocatorUnitTest, test_moving)
{
	SmallFragmentationAllocator A2;
	auto A2_P1 = A2.Alloc(10 * sizeof(int));
	auto A2_P2 = A2.Alloc(10 * sizeof(int));

	for(int i = 0; i < 10; i++)
		((int*)A2_P1)[i] = i;
	for(int i = 0; i < 10; i++)
		EXPECT_EQ(i, ((int*)A2_P1)[i]);

	A2.ReAlloc(A2_P1, 20 * sizeof(int));

	auto temp = (int*)A2_P1;

	auto A2_P3(std::move(A2_P1));
	EXPECT_EQ(temp, (int*)A2_P3);
	for(int i = 0; i < 10; i++)
			EXPECT_EQ(i, ((int*)A2_P3)[i]);

	A2.Free(A2_P3);
	A2.Free(A2_P2);
	EXPECT_ANY_THROW(((int*)A2_P1));
}

TEST(SmallFragmentationAllocatorUnitTest, defragment_empty)
{
	SmallFragmentationAllocator A;
	A.defragment();
}

TEST(SmallFragmentationAllocatorUnitTest, defragment_not_needed)
{
	SmallFragmentationAllocator A;
	auto smartP1 = A.Alloc(10 * sizeof(int));
	auto smartP2 = A.Alloc(10 * sizeof(int));
	auto P1 = (int*)smartP1;
	auto P2 = (int*)smartP2;
	A.defragment();
	EXPECT_EQ(P1, (int*)smartP1);
	EXPECT_EQ(P2, (int*)smartP2);
}

TEST(SmallFragmentationAllocatorUnitTest, defragment_needed)
{
	SmallFragmentationAllocator A;
	auto smartP1 = A.Alloc(10 * sizeof(int));
	auto start = (int*)smartP1;

	auto smartP2 = A.Alloc(10 * sizeof(int));
	for(int i = 0; i < 10; i++)
		((int*)smartP2)[i] = 2;

	A.ReAlloc(smartP1, 20 * sizeof(int));

	auto smartP3 = A.Alloc(40 * sizeof(int));
	for(int i = 0; i < 40; i++)
			((int*)smartP3)[i] = 3;

	A.Free(smartP1);

	EXPECT_EQ(start + 10, (int*)smartP2);
	EXPECT_EQ(start + 40, (int*)smartP3);

	A.defragment();

	EXPECT_EQ(start + 0,  (int*)smartP2);
	EXPECT_EQ(start + 10, (int*)smartP3);

	for(int i = 0; i < 10; i++)
		EXPECT_EQ(2, ((int*)smartP2)[i]);

	for(int i = 0; i < 40; i++)
		EXPECT_EQ(3, ((int*)smartP3)[i]);
}

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

	public:
			class pointer
			{
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
				/*template<class T>
				T& operator[](int i)
				{
					auto it = allocator->pointers.find(id);
					if (it != allocator->pointers.end())
						return (*((T*)it->second) + i);
					throw std::runtime_error("bad pointer");
				}*/

				~pointer()
				{
					allocator->pointers.erase(id);
				}
			private:
				pointer(SmallFragmentationAllocator *allocator, void* ptr) : allocator(allocator), ptr(ptr), id(staticId)
				{
					allocator->pointers.insert(std::make_pair(id, ptr));
					staticId++;
				}
				static size_t staticId;
				void* ptr;
				const size_t id;
				SmallFragmentationAllocator* allocator;
				friend class SmallFragmentationAllocator;
			};

			friend class pointer;

			static const int memorySize = 1048576;

			pointer Alloc(unsigned int Size)
			{
				if (Size > memorySize)
					throw std::bad_alloc();

				if (occupiedMemory.empty())
				{
					occupiedMemory.insert(std::make_pair(0, Size));
					return pointer(this, Memory);
				}

				std::map<size_t, size_t> freeMemory = findFreeBlocks();

				auto it = freeMemory.upper_bound(Size);
				if (it == freeMemory.end())
					throw std::bad_alloc();

				occupiedMemory.insert(std::make_pair(it->second, Size));
				//pointers.insert()
				return pointer(this, Memory + it->second);
			}

			void ReAlloc(pointer& Pointer, unsigned int Size)
			{
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
					Pointer.ptr = Memory + it->first;
				}

				auto retPtr = Alloc(Size);
				memcpy(retPtr, Pointer, it->second);
				Free(Pointer);
				Pointer.ptr = retPtr;
			}

			void Free(pointer& Pointer)
			{
				auto it = occupiedMemory.find((char*)Pointer - Memory);
				if (it != occupiedMemory.end())
					occupiedMemory.erase(it);
			}
	private:
			std::map<size_t, size_t> occupiedMemory;
			std::map<size_t, void*> pointers;
			char Memory[memorySize];
	};
	size_t SmallFragmentationAllocator::pointer::staticId = 0;
}

TEST(SmallFragmentationAllocatorUnitTest, test1)
{
	SmallFragmentationAllocator A1;
	auto A1_P1 = A1.Alloc(sizeof(int));
	A1.ReAlloc(A1_P1, 2 * sizeof(int));
	A1.Free(A1_P1);
}

TEST(SmallFragmentationAllocatorUnitTest, test2)
{
	SmallFragmentationAllocator A2;
	auto A2_P1 = A2.Alloc(10 * sizeof(int));
	for(unsigned int i = 0; i < 10; i++) ((int*)A2_P1)[i] = i;
	for(unsigned int i = 0; i < 10; i++) if(((int*)A2_P1)[i] != i) std::cout << "ERROR 1" << std::endl;

	auto A2_P2 = A2.Alloc(10 * sizeof(int));
	for(unsigned int i = 0; i < 10; i++) ((int*)A2_P2)[i] = -1;
	for(unsigned int i = 0; i < 10; i++) if(((int*)A2_P1)[i] != i) std::cout << "ERROR 2" << std::endl;
	for(unsigned int i = 0; i < 10; i++) if(((int*)A2_P2)[i] != -1) std::cout << "ERROR 3" << std::endl;
	A2.ReAlloc(A2_P1, 20 * sizeof(int));
	for(unsigned int i = 10; i < 20; i++) ((int*)A2_P1)[i] = i;
	for(unsigned int i = 0; i < 20; i++) if(((int*)A2_P1)[i] != i) std::cout << "ERROR 4" << std::endl;
	for(unsigned int i = 0; i < 10; i++) if(((int*)A2_P2)[i] != -1) std::cout << "ERROR 5" << std::endl;
	A2.ReAlloc(A2_P1, 5 * sizeof(int));
	for(unsigned int i = 0; i < 5; i++) if(((int*)A2_P1)[i] != i) std::cout << "ERROR 6" << std::endl;
	for(unsigned int i = 0; i < 10; i++) if(((int*)A2_P2)[i] != -1) std::cout << "ERROR 7" << std::endl;
	A2.Free(A2_P1);
	A2.Free(A2_P2);
}

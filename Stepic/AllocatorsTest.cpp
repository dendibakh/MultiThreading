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

namespace
{
	class SmallAllocator
	{
			std::map<size_t, size_t> findFreeBlocks()
			{
				std::map<size_t, size_t> freeBlocks;
				size_t begin = 0;
				for (auto& block : occupiedMemory)
				{
					if (begin - block.first > 0)
					{
						freeBlocks.insert(std::make_pair(begin - block.first, begin));
					}
					begin = block.first + block.second;
				}
				return freeBlocks;
			}
	public:
			static const int memorySize = 1048576;
			void *Alloc(unsigned int Size)
			{
				if (Size > memorySize)
					return nullptr;
					//throw std::bad_alloc("not enough memory");

				if (occupiedMemory.empty())
				{
					occupiedMemory.insert(std::make_pair(0, Size));
					return Memory;
				}

				std::map<size_t, size_t> freeMemory = findFreeBlocks();

				auto it = freeMemory.upper_bound(Size);
				if (it == freeMemory.end())
					return nullptr;

				return Memory + it->second;
			}

			void *ReAlloc(void *Pointer, unsigned int Size)
			{

			}

			void Free(void *Pointer)
			{
				auto it = occupiedMemory.find((char*)Pointer - Memory);
				if (it != occupiedMemory.end())
					occupiedMemory.erase(it);
			}
	private:
			std::map<size_t, size_t> occupiedMemory;
			char Memory[memorySize];
	};
}

TEST(AllocatorUnitTest, test1)
{
	SmallAllocator A1;
	int * A1_P1 = (int *) A1.Alloc(sizeof(int));
	A1_P1 = (int *) A1.ReAlloc(A1_P1, 2 * sizeof(int));
	A1.Free(A1_P1);
	SmallAllocator A2;
	int * A2_P1 = (int *) A2.Alloc(10 * sizeof(int));
	for(unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
	for(unsigned int i = 0; i < 10; i++) if(A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;
	int * A2_P2 = (int *) A2.Alloc(10 * sizeof(int));
	for(unsigned int i = 0; i < 10; i++) A2_P2[i] = -1;
	for(unsigned int i = 0; i < 10; i++) if(A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
	for(unsigned int i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;
	A2_P1 = (int *) A2.ReAlloc(A2_P1, 20 * sizeof(int));
	for(unsigned int i = 10; i < 20; i++) A2_P1[i] = i;
	for(unsigned int i = 0; i < 20; i++) if(A2_P1[i] != i) std::cout << "ERROR 4" << std::endl;
	for(unsigned int i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;
	A2_P1 = (int *) A2.ReAlloc(A2_P1, 5 * sizeof(int));
	for(unsigned int i = 0; i < 5; i++) if(A2_P1[i] != i) std::cout << "ERROR 6" << std::endl;
	for(unsigned int i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;
	A2.Free(A2_P1);
	A2.Free(A2_P2);
}

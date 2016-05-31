#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <atomic>

using namespace testing;

namespace
{
    const size_t N = 100;
    int sharedCount;
    std::atomic<bool> flag;

    struct Worker
    {
        void operator()(int& individualCount)
        {
		for (size_t i = 0; i < N; ++i)
		{		
			//std::cout << " Worker started" << std::endl;				
			if (flag)
			{
				//std::cout << " Worker is working..." << std::endl;				
				++sharedCount;
				++individualCount;
				flag = true;
			}
		}
		std::cout << " Worker is done. sharedCount = " << sharedCount << std::endl;
        }
    };

    struct Blocker
    {
        void operator()(int& individualCount)
        {
		if (flag)
		{		
			std::this_thread::sleep_for (std::chrono::milliseconds(2000)); 			
			++sharedCount;
			++individualCount;
			flag = !flag;
		}
		std::cout << " Blocker is done. sharedCount = " << sharedCount << std::endl;		
        }
    };
}

TEST(AtomicsUnitTest, 4)
{
	flag = true;
	sharedCount = 0;

	Blocker b;
	int count2 = 0;
	std::thread blocker(b, std::ref(count2));

	std::this_thread::sleep_for (std::chrono::milliseconds(500)); 				

	Worker w;
	int count1 = 0;
	std::thread worker(w, std::ref(count1));

    worker.join();
    blocker.join();
    EXPECT_EQ(count1 + count2, sharedCount);
}


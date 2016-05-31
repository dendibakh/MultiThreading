#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <atomic>

using namespace testing;

namespace
{
    const size_t N = 1000000;
    std::atomic<int> sharedCount;
    std::atomic<bool> flag;

    struct Worker
    {
        void operator()(int& individualCount)
        {
		for (size_t i = 0; i < N / 1000; ++i)
		{		
			while (flag != ID)
				{}	
			++sharedCount;
			++individualCount;
			flag = !ID;
		}
		
        }
	bool ID;
    };

    struct Helper
    {
        void operator()()
        {
		for (size_t i = 0; i < N; ++i)
		{		
			flag = !flag;
		}
		
        }
    };
}

TEST(AtomicsUnitTest, 3)
{
	flag = true;
	sharedCount = 0;

	Worker w1;
	w1.ID = 0;
	int count1 = 0;
	std::thread worker1(w1, std::ref(count1));
	
	Worker w2;
	w2.ID = 1;
	int count2 = 0;	
	std::thread worker2(w2, std::ref(count2));

	Helper h;
	std::thread helper(h);

    worker1.join();
    worker2.join();
    helper.join();
    EXPECT_EQ(count1 + count2, sharedCount);
}


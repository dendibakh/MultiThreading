#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <atomic>

using namespace testing;

namespace
{
    const size_t N = 10000;
    std::atomic<int> sharedCount;
    std::atomic<bool> flag;

    struct Worker
    {
        void operator()(int& individualCount)
        {
		for (size_t i = 0; i < N; ++i)
		{		
			if (flag)
			{
				++sharedCount; // There is a data race on sharedCount
				++individualCount;
				flag = false;
			}	
		}
        }
    };

    struct Helper
    {
        void operator()()
        {
		for (size_t i = 0; i < N; ++i)
		{		
			if (!flag)
				flag = true;
		}
        }
    };
}

TEST(AtomicsUnitTest, 1)
{
	flag = true;
	sharedCount = 0;

	Worker w1;
	int count1 = 0;
	std::thread worker1(w1, std::ref(count1));
	
	Worker w2;
	int count2 = 0;	
	std::thread worker2(w2, std::ref(count2));

	Helper h;
	std::thread helper(h);

	worker1.join();
	worker2.join();
	helper.join();
	EXPECT_EQ(count1 + count2, sharedCount);
}

// With current implementation every time test is failing.
// If I make sharedCount an atomic - test will pass.
// The reason is that atomic is not blocking anything. It means when I read atomic variable and continue some other thread also can read the value.
// But there could be just no race when second thread will write simultaneously.

#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <atomic>

using namespace testing;

namespace
{
    const size_t N = 10000;
    std::atomic<int> sharedCount;

    struct Worker
    {
        void operator()()
        {
		for (size_t i = 0; i < N; ++i)
		{		
			++sharedCount;
		}
		
        }
    };
}

TEST(AtomicsUnitTest, 2)
{
	sharedCount = 0;

	Worker w1;
	std::thread worker1(w1);
	
	Worker w2;
	std::thread worker2(w2);

	Worker w3;
	std::thread worker3(w3);

    worker1.join();
    worker2.join();
    worker3.join();
    EXPECT_EQ(3 * N, sharedCount);
}


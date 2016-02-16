#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <future>

using namespace testing;

namespace
{
    std::future<int> fi;

    struct Producer
    {
        int operator()()
        {
        	return 42;
        }
    };

    struct Consumer
    {
        void operator()()
        {
        	fi.wait();
        	EXPECT_TRUE(fi.valid());
       		EXPECT_EQ(42, fi.get());
       		EXPECT_FALSE(fi.valid());
        }
    };
}

TEST(FuturesTest, GlobalFuture)
{
	Producer p;
	std::packaged_task<int(void)> pt(std::move(p));
	fi = pt.get_future();
	std::thread producer(std::move(pt));

	Consumer c;
	std::thread consumer(c);

	EXPECT_TRUE(producer.joinable());
	producer.join();

	EXPECT_TRUE(consumer.joinable());
	consumer.join();
}

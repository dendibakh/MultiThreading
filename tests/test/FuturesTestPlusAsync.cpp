#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <future>

using namespace testing;

namespace
{
    struct Producer
    {
        int operator()()
        {
        	return 42;
        }
    };

    struct Consumer
    {
        void operator()(std::future<int>& fi)
        {
        	fi.wait();
        	EXPECT_TRUE(fi.valid());
       		EXPECT_EQ(42, fi.get());
       		EXPECT_FALSE(fi.valid());
        }
    };
}

TEST(FuturesTest, PlusAsync)
{
	Producer p;
	std::future<int> fi = std::async(p);

	Consumer c;
	std::thread consumer(c, std::ref(fi));

	EXPECT_TRUE(consumer.joinable());
	consumer.join();
}

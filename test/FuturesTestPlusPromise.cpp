#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <future>

using namespace testing;

TEST(FuturesTest, PlusPromiseInOneThread)
{
	std::promise<int> pi;
	std::future<int> fut;

	fut=pi.get_future();

	pi.set_value(42);

       	EXPECT_TRUE(fut.valid());
	EXPECT_EQ(42, fut.get());
       	EXPECT_FALSE(fut.valid());
}

namespace
{
    struct Producer
    {
        void operator()(std::promise<int>& pi)
        {
        	return pi.set_value(42);
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

TEST(FuturesTest, PlusPromiseInDifferentThreads)
{
	std::promise<int> pi;
	std::future<int> fi = pi.get_future();

	Producer p;
	std::thread producer(p, std::ref(pi));

	Consumer c;
	std::thread consumer(c, std::ref(fi));

	EXPECT_TRUE(producer.joinable());
	producer.join();

	EXPECT_TRUE(consumer.joinable());
	consumer.join();
}

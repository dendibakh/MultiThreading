#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <future>

using namespace testing;

// single futures are invalid after call of get()
// shared futures are valid after multiple get() calls
TEST(FuturesTest, SharedFutures)
{
	std::promise<int> pi;
	std::future<int> fut;

	fut=pi.get_future();

	std::shared_future<int> shfut = fut.share();
	EXPECT_TRUE(!fut.valid());

	pi.set_value(42);
       	
       	EXPECT_TRUE(shfut.valid());
	EXPECT_EQ(42, shfut.get());
       	EXPECT_TRUE(shfut.valid());
       	
      	EXPECT_TRUE(shfut.valid());
	EXPECT_EQ(42, shfut.get());
       	EXPECT_TRUE(shfut.valid());
}

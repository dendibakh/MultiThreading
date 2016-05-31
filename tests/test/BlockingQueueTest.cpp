#include "gtest/gtest.h"
#include "BlockingQueue.h"

using namespace testing;

TEST(BlockingQueueUnitTest, 1)
{
			BlockingQueue bq;
			EXPECT_TRUE(bq.isEmpty());
			EXPECT_FALSE(bq.isFull());
			bq.push(1);
			EXPECT_FALSE(bq.isEmpty());
			EXPECT_FALSE(bq.isFull());
			bq.pop();
			EXPECT_TRUE(bq.isEmpty());
			EXPECT_FALSE(bq.isFull());
			for (int i = 0; i < BlockingQueue::maxElements; i++)
			{
				bq.push(i);
			}
			EXPECT_FALSE(bq.isEmpty());
			EXPECT_TRUE(bq.isFull());
			bq.pop();
			EXPECT_FALSE(bq.isEmpty());
			EXPECT_FALSE(bq.isFull());
			bq.push(9);
			EXPECT_FALSE(bq.isEmpty());
			EXPECT_TRUE(bq.isFull());
			for (int i = 0; i < BlockingQueue::maxElements; i++)
			{
				bq.pop();
			}
			EXPECT_TRUE(bq.isEmpty());
			EXPECT_FALSE(bq.isFull());
}

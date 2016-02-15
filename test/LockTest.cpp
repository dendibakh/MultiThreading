#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace testing;

namespace
{
    std::mutex  m;
    std::condition_variable exclusiveLockGained;
    bool exclusiveLockGainedFlag;
    std::condition_variable tryLockerFinished;
    bool tryLockerFinishedFlag;
    std::atomic_bool flag;

    struct ExclusiveLocker
    {
        void operator()()
        {
		std::unique_lock<std::mutex> exLock(m);
		EXPECT_TRUE(exLock.owns_lock());
		while (flag == false) {}
		exclusiveLockGainedFlag = true;
		exclusiveLockGained.notify_one();
		tryLockerFinished.wait(exLock, [](){ return tryLockerFinishedFlag == true; });
        }
    };

    struct TryLocker
    {
        void operator()()
        {
        	std::unique_lock<std::mutex> tryLock(m, std::try_to_lock);
  		EXPECT_FALSE(tryLock.owns_lock());
  		flag = true;
		exclusiveLockGained.wait(tryLock, [](){ return exclusiveLockGainedFlag == true; });
                tryLock.lock(); // wait leaves tryLock in the state equivalent before calling wait, so we need to lock it.
		tryLockerFinishedFlag = true;
		tryLockerFinished.notify_one();
        }
    };
}

TEST(LockTests, SimpleLock)
{
	flag = false;
	exclusiveLockGainedFlag = false;
	tryLockerFinishedFlag   = false;

	ExclusiveLocker exL;
	TryLocker shL;

	std::thread exLocker(exL);
	std::this_thread::sleep_for (std::chrono::milliseconds(100)); 				
	std::thread shLocker(shL);

	EXPECT_TRUE(exLocker.joinable());
	EXPECT_TRUE(shLocker.joinable());

	exLocker.join();
    	shLocker.join();
}

namespace
{
    std::recursive_mutex rm;

    struct RecursiveLocker
    {
    	void recursiveFunc(int& i)
    	{
    		std::unique_lock<std::recursive_mutex> recLock(rm);
    		if (i > 0)
    		{
	    		i--;
	    		recursiveFunc(i);
	    	}
	    	return;
    	}

        void operator()()
        {
        	int i = 5;
		recursiveFunc(i);
		EXPECT_EQ(0, i);
        }
    };
}

TEST(LockTests, RecursiveLock)
{
	RecursiveLocker rL;
	std::thread locker(rL);
	EXPECT_TRUE(locker.joinable());
	locker.join();
}
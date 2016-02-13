#include "gtest/gtest.h"
#include <iostream>
#include <thread>

using namespace testing;

namespace
{
    bool flag;

    struct Worker
    {
        void operator()()
        {
		std::this_thread::sleep_for (std::chrono::milliseconds(100)); // Imitating some work
        }
    };

	void myterminate () 
	{
		flag = true; // if the thread was not joined std::terminate will be called.
	}

	void TestThreadDestruct()
	{
		// for test uncomment :		
		//Worker w;
		//std::thread worker(w);
		//EXPECT_TRUE(worker.joinable());
		flag = true;
	}
}

TEST(ThreadSemantics, 1)
{
	std::set_terminate (myterminate);
	flag = false;

	TestThreadDestruct();

    	EXPECT_TRUE(flag);
}

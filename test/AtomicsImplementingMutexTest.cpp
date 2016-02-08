#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <numeric>

using namespace testing;

namespace
{
    const int N = 10;
    const int Nthreads = 10;
    int sharedCount;
    std::atomic<int> ID;

    struct Worker
    {
        void operator()(int& individualCount)
        {
		for(auto i = 0; i < N; ++i)
		{			
			while (ID != myID) {}
	
			++sharedCount;
			++individualCount;		
	
			++ID;
			ID = ID % Nthreads;
		}
        }
	int myID;
    };

	void addThread(std::vector<std::thread>& vect, int i, std::vector<int>& counts)
	{
		Worker w;		
		w.myID = i;
		int& ref = counts[i];
		vect.push_back(std::thread(w, std::ref(ref)));
	}
}

// This test emulates the implementation of mutexes with atomics
// No need to protect sharedCount
TEST(AtomicsUnitTest, 5)
{
	ID = 0;
	sharedCount = 0;

	std::vector<std::thread> vect;
	std::vector<int> counts(Nthreads, 0);

	for (int i = 0; i < Nthreads; ++i)	
		addThread(vect, i, counts);

	for (auto &i : vect)	
		i.join();

	for (auto &i : counts)	
		EXPECT_EQ(i, N);	

	int actualCount = std::accumulate(counts.begin(), counts.end(), 0);

    	EXPECT_EQ(actualCount, sharedCount);
}


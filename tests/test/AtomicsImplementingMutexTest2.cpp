#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <numeric>
#include <vector>
#include "Barrier.h"
#include "MutexWithAtomics.h" // This is the attempt to implement mutex with atomics
#include "LockGuard.h"

using namespace testing;

namespace
{
    const int N = 10;
    const int Nthreads = 30;
    int sharedCount;
    Barrier b(Nthreads);
    Mutex m;

    struct Worker
    {
        void operator()(int& individualCount)
        {
        	b.Wait();
		for(auto i = 0; i < N; ++i)
		{			
			Lock_Guard lg(m);

			++sharedCount;
			++individualCount;		
		}
        }
    };

	void addThread(std::vector<std::thread>& vect, int i, std::vector<int>& counts)
	{
		Worker w;		
		int& ref = counts[i];
		vect.push_back(std::thread(w, std::ref(ref)));
	}
}

TEST(AtomicsUnitTest, 6)
{
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

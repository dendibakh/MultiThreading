#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <numeric>
#include <vector>
#include <memory>
#include "Barrier.h"

using namespace testing;

namespace
{
    const int N = 5;
    const int Nthreads = 5;
    std::atomic<int> sharedCount;
    std::vector< std::unique_ptr<Barrier> > barriers;

    struct Worker
    {
        void operator()(int& individualCount)
        {
        	for(auto i = 0; i < N; ++i)
		{			
			++sharedCount;
			++individualCount;		
			barriers[i]->Wait();
			EXPECT_EQ((i + 1) * Nthreads, sharedCount.load());
			std::this_thread::sleep_for (std::chrono::milliseconds(100)); // add this delay in order that everybody can finish their checks.
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

TEST(BarrierTest, 1)
{
	sharedCount = 0;

	std::vector<std::thread> vect;
	std::vector<int> counts(Nthreads, 0);

	for (int i = 0; i < N; ++i)	
		barriers.push_back(std::unique_ptr<Barrier>(new Barrier(Nthreads)));

	for (int i = 0; i < Nthreads; ++i)	
		addThread(vect, i, counts);

	for (auto &i : vect)	
		i.join();

	for (auto &i : counts)	
		EXPECT_EQ(i, N);	

	int actualCount = std::accumulate(counts.begin(), counts.end(), 0);
	

    	EXPECT_EQ(actualCount, sharedCount);
}

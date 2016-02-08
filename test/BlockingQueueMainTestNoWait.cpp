#include "gtest/gtest.h"
#include "BlockingQueue.h"
#include <iostream>

using namespace testing;

namespace
{
class Barrier
{
private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::size_t _count;
public:
    explicit Barrier(std::size_t count) : _count{count} { }
    void Wait()
    {
        std::unique_lock<std::mutex> lock{_mutex};
        if (--_count == 0) {
            _cv.notify_all();
        } else {
            _cv.wait(lock, [this] { return _count == 0; });
        }
    }
};

	BlockingQueue bq;
	Barrier b(2);
	const int N = 1000;

	struct Consumer
    {
        void operator()()
        {
			try
			{
				b.Wait();				
				size_t failureCounter = 0;		                
				for (int i = 0; i < N; ++i)
				{
					try
					{
						bq.popNoWait();
					}
					catch (std::exception& e)
					{
						++failureCounter;
					}				
				}
				//std::cout << " There were " << failureCounter << " pop exceptions " << std::endl;
			}
			catch(std::exception & e)
			{
				std::cout << e.what() << std::endl;
			}
        }
    };

    struct Producer
    {
        void operator()()
        {
            try
			{
				b.Wait();
				size_t failureCounter = 0;		                
                		for (int i = 0; i < N; ++i)
				{
					try
					{
						bq.pushNoWait(i);
					}
					catch (std::exception& e)
					{
						++failureCounter;
					}
				}
				//std::cout << " There were " << failureCounter << " push exceptions " << std::endl;
			}
			catch(std::exception & e)
			{
				std::cout << e.what() << std::endl;
			}
        }
    };
	
    std::thread getConsumerThread()
    {
        Consumer x;
        return std::thread(x);
    }

    std::thread getProducerThread()
    {
        Producer x;
        return std::thread(x);
    }
}


TEST(BlockingQueueUnitTest, 4)
{
std::thread producer = getProducerThread();	
std::thread consumer = getConsumerThread();	

    consumer.join();
    producer.join();
    //std::cout << " The size of queue is " << bq.size() << std::endl;
}


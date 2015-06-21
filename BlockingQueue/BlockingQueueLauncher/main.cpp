#include "BlockingQueue.h"
#include <iostream>

namespace
{
	BlockingQueue bq;

	struct Consumer
    {
        void operator()()
        {
			try
			{
                for (int i = 0; i < 30; ++i)
				{
					if (i == 15) // Let's stop for a short time this thread to see what happens
						boost::this_thread::sleep_for (boost::chrono::milliseconds(10000)); 
					int val = bq.pop();
					std::cout << "        popped " << val << "   size = " << bq.size() << std::endl;
				}
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
                for (int i = 0; i < 30; ++i)
				{
					bq.push(i);
					std::cout << "Pushed         " << i << "   size = " << bq.size() << std::endl;
				}
			}
			catch(std::exception & e)
			{
				std::cout << e.what() << std::endl;
			}
        }
    };
	
    boost::thread getConsumerThread()
    {
        Consumer x;
        return boost::thread(x);
    }

    boost::thread getProducerThread()
    {
        Producer x;
        return boost::thread(x);
    }
}

int main()
{
	boost::thread consumer = getConsumerThread();
	boost::this_thread::sleep_for (boost::chrono::milliseconds(500)); // a little delay
	boost::thread producer = getProducerThread();

    consumer.join();
    producer.join();
	/*
	Output:

	Pushed         0   size = 1 
			popped 0   size = 0
	Pushed         1   size = 1
			popped 1   size = 0
	Pushed         2   size = 1
			popped 2   size = 0
	Pushed         3   size = 1
			popped 3   size = 0
	Pushed         4   size = 1
	Pushed         5   size = 2
			popped 4   size = 1
			popped 5   size = 0
	Pushed         6   size = 1
	Pushed         7   size = 2
			popped 6   size = 1
			popped 7   size = 0
	Pushed         8   size = 1
			popped 8   size = 0
	Pushed         9   size = 1
	Pushed         10   size = 2
			popped 9   size = 1
			popped 10   size = 0
	Pushed         11   size = 1
	Pushed         12   size = 2
			popped 11   size = 1
	Pushed         13   size = 2
			popped 12   size = 1
	Pushed         14   size = 2
			popped 13   size = 1
	Pushed         15   size = 2
			popped 14   size = 1
	Pushed         16   size = 2
	Pushed         17   size = 3
	Pushed         18   size = 4
	Pushed         19   size = 5
	Pushed         20   size = 6
	Pushed         21   size = 7
	Pushed         22   size = 8
	Pushed         23   size = 9
	Pushed         24   size = 10
			popped 15   size = 9
	Pushed         25   size = 10
			popped 16   size = 9
			popped 17   size = 8
	Pushed         26   size = 9
			popped 18   size = 8
	Pushed         27   size = 9
			popped 19   size = 8
	Pushed         28   size = 9
			popped 20   size = 8
			popped 21   size = 7
	Pushed         29   size = 8
			popped 22   size = 7
			popped 23   size = 6
			popped 24   size = 5
			popped 25   size = 4
			popped 26   size = 3
			popped 27   size = 2
			popped 28   size = 1
			popped 29   size = 0
	*/
}
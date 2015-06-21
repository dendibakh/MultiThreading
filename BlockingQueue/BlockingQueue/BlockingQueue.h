#pragma once
#include <boost/thread/thread.hpp>
#include <queue>

class BlockingQueue
{
public:
	void push(int value)
	{
		boost::unique_lock<boost::mutex> lock(mut);
		while (queue_full)
		{
			full.wait(lock);
		}
		
		if (q.size() == maxElements)
			throw std::runtime_error("Nowhere to add");
		q.push(value);
		queue_empty = false;
		if (q.size() == maxElements)
			queue_full = true;
		else
			queue_full = false;
		empty.notify_one();
	}
	int pop()
	{
		boost::unique_lock<boost::mutex> lock(mut);
		while (queue_empty)
		{
			empty.wait(lock);
		}

		if (q.empty())
			throw std::runtime_error("Nothing to pop");
		int retval = q.front();
		q.pop();
		queue_full = false;
		if (q.empty())
			queue_empty = true;
		else
			queue_empty = false;
		full.notify_one();
		return retval;
	}
	bool isEmpty() { return queue_empty; }
	bool isFull()  { return queue_full;  }
	int size()     { return q.size();    }
	BlockingQueue() : queue_empty(true), queue_full(false)
	{}
private:
	static const int maxElements = 10;
	std::queue<int> q;
	boost::mutex mut;
	boost::condition_variable empty;
	boost::condition_variable full;
	bool queue_empty;
	bool queue_full;
};
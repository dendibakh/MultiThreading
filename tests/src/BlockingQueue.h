#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class BlockingQueue
{
public:
	void push(int value)
	{
		std::unique_lock<std::mutex> lock(mut);
		while (isFull())
		{
			full.wait(lock);
		}
		
		if (isFull())
			throw std::runtime_error("Nowhere to add");
		q.push(value);
		empty.notify_one();
	}
	void pushNoWait(int value)
	{
		std::unique_lock<std::mutex> lock(mut);
		if (isFull())
			throw std::runtime_error("Nowhere to add");
		
		q.push(value);
	}
	int pop()
	{
		std::unique_lock<std::mutex> lock(mut);
		while (isEmpty())
		{
			empty.wait(lock);
		}

		if (isEmpty())
			throw std::runtime_error("Nothing to pop");
		int retval = q.front();
		q.pop();
		full.notify_one();
		return retval;
	}
	int popNoWait()
	{
		std::unique_lock<std::mutex> lock(mut);
		if (isEmpty())
			throw std::runtime_error("Nothing to pop");

		int retval = q.front();
		q.pop();
		return retval;
	}
	bool isEmpty() { return q.empty(); }
	bool isFull()  { return q.size() == maxElements;  }
	int size()     { return q.size();    }
	BlockingQueue() {}
	static const int maxElements = 100;
private:
	std::queue<int> q;
	std::mutex mut;
	std::condition_variable empty;
	std::condition_variable full;
};

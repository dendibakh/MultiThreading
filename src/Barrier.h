#pragma once
#include <mutex>
#include <condition_variable>
#include <atomic>

class Barrier
{
public:
    explicit Barrier(size_t count) : count(count) { }
    void Wait()
    {
        std::unique_lock<std::mutex> lock(mutex);
	--count;
        if (count == 0) 
	{
            cv.notify_all();
        } 
	else 
	{
            cv.wait(lock, [this] { return count == 0; });
	    // equivalent to:
	    // while (count != 0)
            //    cv.wait(lock);
        }
    }
private:
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<size_t> count;
};

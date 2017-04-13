#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

/*
  Wrapper around a queue.
  Allows a series of tasks to be performed.
  Will always be threadsafe.
*/
template <typename T>
class ThreadQueue
{
public:
	//Constructor not needed. Will act as a collection.

	~ThreadQueue()
	{
		invalidate();
	}

	/*
	  Get the first value in the queue...
	  Will block until the value is available.
	   -- unless clear or destructor is called.
	  Return true if succesful.
	*/
	bool WaitPop(T& out)
	{
		unique_lock<mutex> lock(tMutex);

		condition.wait(lock, [this]() {
				return !tQueue.empty() || !valid;
		});

		if (!valid) return false;

		out = move(tQueue.front());
		tQueue.pop();

		return true;
	}

	//Insert a new value
	void Push(T value) 
	{
		unique_lock<mutex> lock(tMutex);

		tQueue.push(move(value));
		condition.notify_one();
	}

	/*<---NO THREADS CAN CALL OTHER FUNCTIONS 
	WHILE THSE ARE ACTIVE--->*/

	//Is queue empty?
	bool IsEmpty() const
	{
		lock_guard<mutex> lock(tMutex);

		return tQueue.empty();
	}

	//Get rid of everything!
	void DeleteAll()
	{
		lock_guard<mutex> lock(tMutex);

		while (!tQueue.empty()) {
			tQueue.pop();
		}

		condition.notify_all();
	}

	/*
	  Ensure nothing is being waited on, if a thread is trying to exit. 
	  Will be an error to continue using it after calling this.
	*/
	void invalidate()
	{
		lock_guard<mutex> lock(tMutex);

		valid = false;
		condition.notify_all();
	}

		
	bool isValid() const
	{
		lock_guard<mutex> lock(tMutex);

		return valid;
	}

	/*<--------------------->*/

private:
	std::atomic_bool valid{ true };

	mutable std::mutex		tMutex;
	std::queue<T>			tQueue;
	std::condition_variable condition;
};
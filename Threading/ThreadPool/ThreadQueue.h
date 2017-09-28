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
		Invalidate();
	}

	/*
	  Get the first value in the queue...
	  Will block until the value is available.
	   -- unless clear or destructor is called.
	  Return true if succesful.
	*/
	bool WaitPop(T& out)
	{
		unique_lock<mutex> lock(function_mutex);

		condition.wait(lock, [this]() {
			return !queue_instance.empty() || !valid;
		});

		if (!valid) return false;

		out = move(queue_instance.front());
		queue_instance.pop();

		return true;
	}

	//Insert a new value
	void Push(T value) 
	{
		unique_lock<mutex> lock(function_mutex);

		queue_instance.push(move(value));
		condition.notify_one();
	}

	/*<---NO THREADS CAN CALL OTHER FUNCTIONS 
	WHILE THSE ARE ACTIVE--->*/

	//Is queue empty?
	bool IsEmpty() const
	{
		lock_guard<mutex> lock(function_mutex);

		return instance.empty();
	}

	//Get rid of everything!
	void DeleteAll()
	{
		lock_guard<mutex> lock(function_mutex);

		while (!queue_instance.empty()) {
			queue_instance.pop();
		}

		condition.notify_all();
	}

	/*
	  Ensure nothing is being waited on, if a thread is trying to exit. 
	  Will be an error to continue using it after calling this.
	*/
	void Invalidate()
	{
		lock_guard<mutex> lock(function_mutex);

		valid = false;
		condition.notify_all();
	}

		
	bool IsValid() const
	{
		lock_guard<mutex> lock(function_mutex);

		return valid;
	}

	/*<--------------------->*/

private:
	std::atomic_bool valid{ true };

	mutable std::mutex		function_mutex;
	std::queue<T>			queue_instance;
	std::condition_variable condition;
};
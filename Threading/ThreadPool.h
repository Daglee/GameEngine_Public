#pragma once

#include "ThreadQueue.h"
#include "ThreadTask.h"
#include "TaskFuture.h"
#include "../ResourceManagment/ResourceBase.h"
#include "../nclgl/Window.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

/*
  Bunch of threads ready to carry tasks out.
*/
class ThreadPool : public ResourceBase
{
public:
	/*
	  Create a thread pool with the maximum number of threads available 
	  on the current machine.
	  Subtract 1 (for main thread). 
	  Therefore a check is needed to ensure there is always at least 1 thread.
	*/
	ThreadPool();

	//Number of threads is previously known...
	ThreadPool(const int numThreads);

	~ThreadPool() {
		destroy();
	}

	/*
	  Give the thread pool a task to do.
	  Tidies up the private one a bit + it's inline
	*/
	template <typename Function, typename... Params>
	inline auto SubmitJob(Function&& func, Params&&... params)
	{
		/*
		  If thread execution is paused, no job will be 
		  submitted to maintain the current state.
		*/
		if (!paused) return Submit(forward<Function>(func), forward<Params>(params)...);
	}

	void CheckPause()
	{
		if (Window::GetKeyboard()->KeyTriggered(pauseButton)) paused = !paused;
	}

	void Read(string resourcename);
	void ReadParams(string params);

	std::atomic_bool paused = false;
	KeyboardKeys pauseButton;
private:
	/*
	  Get a thread to carry out a function/method
	  Using auto because its easier and reads better...
	*/
	template <typename Function, typename... Params>
	auto ThreadPool::Submit(Function&& func, Params&&... args) 
	{
		/*
		  Create a function template with the passed in function 
		  reference and any number of parameters.
		*/
		auto thisTask = bind(forward<Function>(func), forward<Params>(args)...);

		using ResultType	= result_of_t<decltype(thisTask)()>; //What will the function return?
		using PackagedTask	= packaged_task<ResultType()>;		 //Prepare the function for async...
		using TaskType		= ThreadTask<PackagedTask>;			 //Create a thread task for that function.

		//So fancy things can be done.
		PackagedTask			task{ move(thisTask) };
		TaskFuture<ResultType>	result{ task.get_future() };

		//Let a thread carry the task out.
		taskQueue.Push(make_unique<TaskType>(move(task)));
		return result;
	}

	//Initialise threads
	void CreateWorkers(int numWorkers);

	//Threads use this continiously and grab a task to perform.
	void worker();

	//Invalidate queue and join all threads.
	void destroy();

	std::atomic_bool finished; //Should the threads still be finding tasks to carry out?
	ThreadQueue<std::unique_ptr<Task>> taskQueue;
	std::vector < std::thread > threads;
};

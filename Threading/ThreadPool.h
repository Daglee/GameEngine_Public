#pragma once

#include "ThreadQueue.h"
#include "ThreadTask.h"
#include "TaskFuture.h"
#include "../ResourceManagment/Resource.h"
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
class ThreadPool : public Resource
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
		running = false;
		taskQueue.Invalidate();

		JoinAll();
	}

	/*
	  Give the thread pool a task to do y getting
	  a thread to carry out a function/method.
	  Using auto because its easier and reads better...
	*/
	template <typename Function, typename... Params>
	auto SubmitJob(Function&& func, Params&&... params)
	{
		/*
		  Create a function template with the passed in function
		  reference and any number of parameters.
		*/
		auto thisTask = bind(forward<Function>(func), forward<Params>(params)...);

		using resultType = result_of_t<decltype(thisTask)()>; //What will the function return?
		using packagedTask = packaged_task<resultType()>;		 //Prepare the function for async...
		using taskType = ThreadTask<packagedTask>;			 //Create a thread task for that function.

															 //So fancy things can be done.
		packagedTask			task{ move(thisTask) };
		TaskFuture<resultType>	result{ task.get_future() };

		//Let a thread carry the task out.
		taskQueue.Push(make_unique<taskType>(move(task)));
		return result;
	}

	inline bool Paused()
	{
		if (Window::GetKeyboard()->KeyTriggered(pauseButton)) paused = !paused;
		return paused;
	}

	void Read(const string resourcename) override;
	void ReadParams(const string params) override;

	KeyboardKeys pauseButton;
private:
	std::atomic_bool paused = false;

	//Initialise threads
	void InitialiseWorkers(int numWorkers);

	//Threads use this continiously and grab a task to perform.
	void FindNewTask();

	//Invalidate queue and join all threads.
	void JoinAll();

	std::atomic_bool running; //Should the threads still be finding tasks to carry out?
	ThreadQueue<std::unique_ptr<Task>> taskQueue;
	std::vector<std::thread> threads;
};

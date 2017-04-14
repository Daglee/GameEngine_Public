#include "ThreadPool.h"

ThreadPool::ThreadPool(const int numThreads) : ResourceBase() 
{
	running = true;
	InitialiseWorkers(numThreads);

	this->SetResourceSize(sizeof(*this));
}

ThreadPool::ThreadPool() : ResourceBase()
{
	running = true;

	//Create as many threads as the hardware allows but keep one for the main thread.
	int numThreads = (max)(thread::hardware_concurrency(), unsigned(2)) - 1;

	InitialiseWorkers(numThreads);

	this->SetResourceSize(sizeof(*this));
}

void ThreadPool::InitialiseWorkers(int numWorkers)
{
	try {
		for (int i = 0; i < numWorkers; ++i) {
			threads.emplace_back(&ThreadPool::FindNewTask, this);
		}
	}
	catch (...) {
		//Don't continue.
		running = false;
		taskQueue.Invalidate();

		JoinAll();

		throw;
	}
}

void ThreadPool::FindNewTask()
{
	//Continiously poll the queue until all work is done
	while (running) {
		unique_ptr<Task> newTask = nullptr;

		if (taskQueue.WaitPop(newTask)) {
			//A new job has been found. Put it on a thread and do it...
			newTask->execute();
		}
	}
}

void ThreadPool::JoinAll()
{
	//Wont let me do my usual "for each"...
	for (auto& thread : threads) {
		if (thread.joinable()) {
			thread.join(); //Stop all threads.
		}
	}
}

void ThreadPool::Read(string resourcename)
{
	this->SetResourceName(resourcename);
}

void ThreadPool::ReadParams(string params) 
{
	Read(params);
}
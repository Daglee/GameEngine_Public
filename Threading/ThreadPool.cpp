#include "ThreadPool.h"

ThreadPool::ThreadPool(const int numThreads) : ResourceBase() 
{
	finished = false;
	CreateWorkers(numThreads);

	this->SetResourceSize(sizeof(*this));
}

ThreadPool::ThreadPool() : ResourceBase()
{
	finished = false;

	//Create as many threads as the hardware allows but keep one for the main thread.
	int numThreads = (max)(thread::hardware_concurrency(), unsigned(2)) - 1;

	CreateWorkers(numThreads);

	this->SetResourceSize(sizeof(*this));
}

void ThreadPool::CreateWorkers(int numWorkers) 
{
	try {
		for (int i = 0; i < numWorkers; ++i) {
			threads.emplace_back(&ThreadPool::worker, this);
		}
	}
	catch (...) {
		//Dont continue.
		destroy();
		throw;
	}
}

void ThreadPool::worker()
{
	//Continiously poll the queue until all work is done
	while (!finished) {
		unique_ptr<Task> pTask = nullptr;

		if (taskQueue.WaitPop(pTask)) {
			//A new job has been found. Put it on a thread and do it...
			pTask->execute();
		}
	}
}

void ThreadPool::destroy()
{
	finished = true; 
	taskQueue.invalidate();

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
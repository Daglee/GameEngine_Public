#include "ThreadPool.h"

ThreadPool::ThreadPool(const int numThreads) : Resource() 
{
	running = true;
	InitialiseWorkers(numThreads);

	this->SetSizeInBytes(sizeof(*this));
}

ThreadPool::ThreadPool() : Resource()
{
	running = true;

	//Create as many threads as the hardware allows but keep one for the main thread.
	const int numThreads = (max)(thread::hardware_concurrency(), unsigned(2)) - 1;

	InitialiseWorkers(numThreads);

	this->SetSizeInBytes(sizeof(*this));
}

void ThreadPool::InitialiseWorkers(int numWorkers)
{
	for (int i = 0; i < numWorkers; ++i) {
		threads.emplace_back(&ThreadPool::FindNewTask, this);
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
	for (auto& thread : threads) {
		if (thread.joinable()) {
			thread.join(); //Stop all threads.
		}
	}
}

void ThreadPool::Read(const string resourcename)
{
	this->SetName(resourcename);
}

void ThreadPool::ReadParams(const string params)
{
	Read(params);
}
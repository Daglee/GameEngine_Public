#include "ThreadManager.h"

ThreadManager::ThreadManager(int maxThreads) : ResourceBase()
{
	maxNumThreads = maxThreads;
	threads = new thread[maxThreads];

	this->SetResourceSize(sizeof(*this));
}


ThreadManager::~ThreadManager()
{
	delete threads;
}

int ThreadManager::AvailableThread() {
	bool threadAvailable = false;

	while (!threadAvailable) {
		for (int i = 0; i < maxNumThreads; ++i) {
			if (!threads[i].joinable()) {
				threadAvailable = true;
				activeThreads.push_back(i);
				return i;
			}
		}
	}
	Log::Error("FAILED TO FIND AVAILABLE THREAD");

	return -1;
}

void ThreadManager::SuspendActiveThreads() {
	for (vector<int>::iterator i = activeThreads.begin(); i != activeThreads.end();) {
		HANDLE h_thread = threads[(*i)].native_handle();

		SuspendThread(h_thread);
		suspendedThreads.push_back((*i));
		i = activeThreads.erase(i);
	}
}

void ThreadManager::ResumeActiveThreads() {
	for (vector<int>::iterator i = suspendedThreads.begin(); i != suspendedThreads.end(); i++) {
		HANDLE h_thread = threads[(*i)].native_handle();
	//	HANDLE h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, threads[(*i)].get_id());

		ResumeThread(h_thread);
		activeThreads.push_back(suspendedThreads[(*i)]);
	}

	suspendedThreads.clear();
}

void ThreadManager::JoinActiveThreads() {
	for (vector<int>::iterator i = activeThreads.begin(); i != activeThreads.end(); ++i) {
		threads[(*i)].join();
		//threads[(*i)].detach();
	}

	activeThreads.clear();
}

void ThreadManager::DetachActiveThreads() {
	for (vector<int>::iterator i = activeThreads.begin(); i != activeThreads.end(); ++i) {
		//threads[(*i)].join();
		threads[(*i)].detach();
	}

	activeThreads.clear();
}

void ThreadManager::Read(string resourcename) {
	this->SetResourceName(resourcename);
}

void ThreadManager::ReadParams(string params) {
	std::istringstream iss(params);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	string name = tokens.at(0);
	int maxNumThreads = atoi(tokens.at(1).c_str());
	threads = new thread[maxNumThreads];
	this->SetResourceName(name);
	this->SetResourceSize(sizeof(*this));
}
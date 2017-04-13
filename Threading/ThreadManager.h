#pragma once

#include "../ResourceManagment/ResourceBase.h"
#include "../ResourceManagment/Log.h"
#include <Windows.h>
#include <thread>
#include <vector>

using namespace std;

class ThreadManager : public ResourceBase
{
public:
	ThreadManager(int maxThreads = 0);
	~ThreadManager();

	int AvailableThread();

	void SuspendActiveThreads();
	void ResumeActiveThreads();

	void JoinActiveThreads();
	void DetachActiveThreads();

	void Read(string resourcename);
	void ReadParams(string params);

	thread* threads;

private:
	int maxNumThreads;
	vector<int> activeThreads;
	vector<int> suspendedThreads;
};


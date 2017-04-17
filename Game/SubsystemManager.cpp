#include "SubsystemManager.h"

SubsystemManager::SubsystemManager(
	InputManager* inputManager, ThreadPool* threadPool,
	Renderer* renderer, Window* window, FSMManager* gamelogic, 
	PhysicsEngine* physicsEngine, Profiler* profiler) 
{
	this->inputManager	= inputManager;
	this->renderer		= renderer;
	this->window		= window;
	this->gamelogic		= gamelogic;
	this->physicsEngine = physicsEngine;
	this->profiler		= profiler;
	this->threadPool	= threadPool;

	subsystems[0] = inputManager;
	subsystems[1] = gamelogic;
	subsystems[2] = physicsEngine;
	subsystems[3] = profiler;
	subsystems[4] = AudioManager::GetInstance();
}

//Game loop iteration!
void SubsystemManager::ThreadedUpdate(float deltatime)
{
	/*
	  The pause button MUST be checked via the thread pool
	  itself as the UI System will be suspended and no more
	  updates will be performed on it.
	*/
	threadPool->CheckPause();

	//OpenGL doesn't like to be threaded...
	renderer->Update(deltatime);

	//But the other subsystems do...
	vector<TaskFuture<void>> updates;
	for each(Subsystem* subsystem in subsystems)
	{
		if (!threadPool->paused) {
			updates.push_back(threadPool->SubmitJob([](	//A promise that it will get done...
				Subsystem* s, float dt) {				//Any parameters needed...
				s->Update(dt);							//The function to call...
			}, subsystem, deltatime));					//What we are passing in...
		}
	}
	//Equivalent to "physicsEngine->Update(deltatime);".

	//Synchronise threads.
	for (auto& task : updates) {
		task.Complete();
	}
}

void SubsystemManager::Update(float deltatime)
{
	renderer->Update(deltatime);

	for each(Subsystem* subsystem in subsystems)
	{
		subsystem->Update(deltatime);
	}
}
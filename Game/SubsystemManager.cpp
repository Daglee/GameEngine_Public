#include "SubsystemManager.h"

#include "../ResourceManagment/DataBase.h"

SubsystemManager::SubsystemManager(DataBase* database)
{
	this->renderer		= database->GRenderer->Find("Renderer");
	this->threadPool	= database->GThreadPool->Find("ThreadPool");

	subsystems[INPUT_MANAGER]	= database->GInputManager->Find("InputManager");
	subsystems[GAME_LOGIC]		= database->GFSMManager->Find("GFSMManager");
	subsystems[PHYS_ENGINE]		= database->GPhysicsEngine->Find("PhysicsEngine");
	subsystems[PROFILER]		= database->GProfiler->Find("Profiler");
	subsystems[AUDIO_MNGR]		= AudioManager::GetInstance();
}

//Game loop iteration!
void SubsystemManager::ThreadedUpdate(float deltatime)
{
	//OpenGL doesn't like to be threaded...
	renderer->Update(deltatime);

	//But these other subsystems do...

	/*
	  The promises that multithreaded updates will get 
	  done must be stored as they would wait and join
	  if they went out of scope/destructor was called.
	*/
	vector<TaskFuture<void>> updates;

	if (!threadPool->Paused()) {
		//Start all the threads needed to update sybsystems...
		for each(Subsystem* subsystem in subsystems)
		{
			updates.push_back(threadPool->SubmitJob([](Subsystem* s, float dt) {	//Any parameters needed...
				s->Update(dt);				//The function to call...
			}, subsystem, deltatime));		//What we are passing in...
			//Equivalent to "physicsEngine->Update(deltatime)".
		}
	}

	//Synchronise threads.
	for (auto& task : updates) {
		task.Complete();
	}
}

void SubsystemManager::Update(float deltatime)
{
	renderer->Update(deltatime);

	/*
	  Even though this update is not threaded,
	  this works just fine and saves writing
	  the same code in two places.
	*/
	if (!threadPool->Paused()) {
		for each(Subsystem* subsystem in subsystems)
		{
			subsystem->Update(deltatime);
		}
	}

}
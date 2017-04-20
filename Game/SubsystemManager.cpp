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
	/*
	  The pause button MUST be checked via the thread pool
	  itself as the UI System will be suspended and no more
	  updates will be performed on it.

	  The UI System start up will specify the button to check.
	*/

	//OpenGL doesn't like to be threaded...
	renderer->Update(deltatime);

	//But the other subsystems do...

	//Contains the promises that the updates will get done.
	vector<TaskFuture<void>> updates;

	//Update each subsystem...
	if (!threadPool->Paused()) {
		for each(Subsystem* subsystem in subsystems)
		{
			updates.push_back(threadPool->SubmitJob([](Subsystem* s, float dt) {	//Any parameters needed...
				s->Update(dt);				//The function to call...
			}, subsystem, deltatime));		//What we are passing in...
		}
	}
	//Equivalent to "physicsEngine->Update(deltatime)".

	//Synchronise threads.
	for (auto& task : updates) {
		task.Complete();
	}
}

void SubsystemManager::Update(float deltatime)
{
	/*
	  Even though this update i not threaded, 
	  this works just fine and saves writing 
	  the same code in two places.
	*/
	threadPool->Paused();

	renderer->Update(deltatime);

	if (!threadPool->paused) {
		for each(Subsystem* subsystem in subsystems)
		{
			subsystem->Update(deltatime);
		}
	}

}
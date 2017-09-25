#include "SubsystemManager.h"

#include "../ResourceManagment/DataBase.h"
#include "../GameLogicFSM/MessageSystem.h"
#include "../UISystem/InputManager.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../Physics/PhysicsEngine.h"
#include "../Profiler/Profiler.h"
#include "../Threading/ThreadPool.h"
#include "../nclgl/Renderer.h"

enum SUBSYSTEM_INDEXES
{
	INPUT_MANAGER,
	GAME_LOGIC,
	PHYS_ENGINE,
	PROFILER,
	AUDIO_MNGR
};

const int MESSAGE_LIFETIME = 100;

SubsystemManager::SubsystemManager(DataBase* database)
{
	this->renderer = static_cast<Renderer*>(database->GetTable("GRenderer")->GetResources()->Find("Renderer"));
	this->threadPool = static_cast<ThreadPool*>(database->GetTable("GThreadPool")->GetResources()->Find("ThreadPool"));

	subsystems[INPUT_MANAGER] = static_cast<InputManager*>(database->GetTable("GInputManager")->GetResources()->Find("InputManager"));
	subsystems[GAME_LOGIC] = static_cast<FSMManager*>(database->GetTable("GFSMManager")->GetResources()->Find("GFSMManager"));
	subsystems[PHYS_ENGINE] = static_cast<PhysicsEngine*>(database->GetTable("PhysicsEngine")->GetResources()->Find("PhysicsEngine"));
	subsystems[PROFILER] = static_cast<Profiler*>(database->GetTable("GProfiler")->GetResources()->Find("Profiler"));
	subsystems[AUDIO_MNGR] = AudioManager::GetInstance();
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

	if (!threadPool->Paused())
	{
		//Start all the threads needed to update sybsystems...
		for each(Subsystem* subsystem in subsystems)
		{
			updates.push_back(threadPool->SubmitJob([](Subsystem* s, float dt)
			{	//Any parameters needed...
				s->Update(dt);				//The function to call...
			}, subsystem, deltatime));		//What we are passing in...
			//Equivalent to "physicsEngine->Update(deltatime)".
		}
	}

	//Synchronise threads.
	for (auto& task : updates)
	{
		task.Complete();
	}

	//if (updateCount % MESSAGE_LIFETIME == 0) MessageSystem::GetInstance()->ClearAllMessages();

	MessageSystem::GetInstance()->ClearAllMessages();
	++updateCount;
}

void SubsystemManager::Update(float deltatime) const
{
	renderer->Update(deltatime);

	/*
	  Even though this update is not threaded,
	  this works just fine and saves writing
	  the same code in two places.
	*/
	if (!threadPool->Paused())
	{
		for each(Subsystem* subsystem in subsystems)
		{
			subsystem->Update(deltatime);
		}
	}


	MessageSystem::GetInstance()->ClearAllMessages();
}
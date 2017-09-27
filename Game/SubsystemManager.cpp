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

	vector<TaskFuture<void>> updates;

	if (!threadPool->Paused())
	{
		for each(Subsystem* subsystem in subsystems)
		{
			updates.push_back(threadPool->SubmitJob([](Subsystem* s, float dt)
			{
				s->Update(dt);				
			}, subsystem, deltatime));		
		}
	}

	for (auto& task : updates)
	{
		task.Complete();
	}

	MessageSystem::GetInstance()->ClearAllMessages();
}
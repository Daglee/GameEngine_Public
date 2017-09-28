#include "SubsystemManager.h"

#include "../ResourceManagement/Database/DataBase.h"
#include "../GameLogicFSM/Messaging/MessageSystem.h"
#include "../UISystem/Input Managment/Input/InputManager.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../Physics/Engine/PhysicsEngine.h"
#include "../Profiler/Profiler.h"
#include "../Threading/ThreadPool/ThreadPool.h"
#include "../nclgl/Rendering/Renderer.h"

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
#include "Launcher.h"
#include "../ResourceManagment/DataBase.h"
#include "ScoreBoard.h"
#include "../Profiler/Profiler.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../Physics/PhysicsEngine.h"
#include "../nclgl/Renderer.h"

Launcher::Launcher(const string rendName, const string windName, DataBase* db)
{
	database = db;
	rendererName = rendName;
	windowName = windName;
}

void Launcher::Launch(const string directory)
{
	if (!GraphicsExists())
	{
		database->StartUp(directory, false, true);

		AttachGraphicsAndInput();
	}

	initialised = IsGraphicsInitialised();
}

void Launcher::InitProfilerTimers() const
{
	//Retrieve everything from the database and hook it up.
	Profiler* profiler = static_cast<Profiler*>(database->GetTable("GProfiler")->GetResources()->Find("Profiler"));
	PhysicsEngine* physicsEngine = static_cast<PhysicsEngine*>(database->GetTable("PhysicsEngine")->GetResources()->Find("PhysicsEngine"));
	FSMManager* fsmManager = static_cast<FSMManager*>(database->GetTable("GFSMManager")->GetResources()->Find("GFSMManager"));

	profiler->AddSubSystemTimer("Renderer", &renderer->updateTimer);
	profiler->AddSubSystemTimer("Input", &inputManager->updateTimer);
	profiler->AddSubSystemTimer("Audio", &AudioManager::GetInstance()->updateTimer);
	profiler->AddSubSystemTimer("PhysicsEngine", &physicsEngine->updateTimer);
	profiler->AddSubSystemTimer("Gamelogic", &fsmManager->updateTimer);
}

bool Launcher::GraphicsExists()
{
	bool systemsExist = true;

	if (!IsSubSystemNull(renderer, "Renderer"))
	{
		systemsExist = false;
	}

	if (window == nullptr)
	{
		Log::Warning("Window is null.");
		systemsExist = false;
	}

	return systemsExist;
}

bool Launcher::IsGraphicsInitialised()
{
	if (!window->HasInitialised())
	{
		Log::Error("Window not succesfully initialised.");
		return false;
	}

	if (!renderer->HasInitialised())
	{
		Log::Error("Renderer not succesfully initialised.");
		return false;
	}

	return true;
}

bool Launcher::IsSubSystemNull(Subsystem* subsystem, const string name)
{
	if (subsystem == nullptr)
	{
		Log::Warning(name + " is null.");
		return true;
	}
	else return false;
}

void Launcher::AttachGraphicsAndInput()
{
	renderer = static_cast<Renderer*>(database->GetTable("GRenderer")->GetResources()->Find(rendererName));
	window = static_cast<Window*>(database->GetTable("GWindow")->GetResources()->Find(windowName));
	inputManager = static_cast<InputManager*>(database->GetTable("GInputManager")->GetResources()->Find("InputManager"));
}
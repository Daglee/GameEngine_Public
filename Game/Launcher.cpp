#include "Launcher.h"
#include "../ResourceManagment/DataBase.h"
#include "ScoreBoard.h"

Launcher::Launcher(string rendName, string windName, DataBase* db)
{
	database = db;
	rendererName = rendName;
	windowName = windName;
}

void Launcher::Launch(string filename)
{
	if (!GraphicsExists())
	{
		database->StartUp(filename, false, true);

		AttachGraphicsAndInput();
	}

	initialised = IsGraphicsInitialised();
}

void Launcher::InitProfilerTimers()
{
	//Retrieve everything from the database and hook it up.
	Profiler* profiler = database->GProfiler->Find("Profiler");

	profiler->AddSubSystemTimer("Renderer", &renderer->updateTimer);
	profiler->AddSubSystemTimer("Input", &inputManager->updateTimer);
	profiler->AddSubSystemTimer("Audio", &AudioManager::GetInstance()->updateTimer);

	profiler->AddSubSystemTimer("PhysicsEngine",
		&database->GPhysicsEngine->Find("PhysicsEngine")->updateTimer);
	profiler->AddSubSystemTimer("Gamelogic",
		&database->GFSMManager->Find("GFSMManager")->updateTimer);

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
}

bool Launcher::IsSubSystemNull(Subsystem* subsystem, std::string name)
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
	renderer = database->GRenderer->Find(rendererName);
	window = database->GWindow->Find(windowName);
	inputManager = database->GInputManager->Find("InputManager");
}
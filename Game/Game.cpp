#include "Game.h"
#include "../ResourceManagment/DataBase.h"
#include "ScoreBoard.h"

Game::Game(Renderer* rend, Window* wind, DataBase* db)
{
	window			= wind;
	renderer		= rend;
	database		= db;
	currentLevel	= nullptr;

	if (window->HasInitialised() && renderer->HasInitialised()) {
		initialised = true;
	}
	else Log::Error("Window or Renderer has failed to initialise.");
}

Game::Game(string rendName, string windName, DataBase* db)
{
	database		= db;
	rendererName	= rendName;
	windowName		= windName;
	currentLevel	= nullptr;

	database->InitialiseDataBase();
}

void Game::StartUp(string filename)
{
	if (renderer == nullptr || window == nullptr) {
		database->StartUp(filename, false, true);

		renderer	 = database->GRenderer->Find(rendererName);
		window		 = database->GWindow->Find(windowName);
		inputManager = database->GInputManager->Find("InputManager");
	}
	else Log::Warning("Database has already been initialised. No startup is required");

	if (window->HasInitialised() && renderer->HasInitialised()) {
		initialised = true;
	}
	else Log::Error("Window or Renderer has failed to initialise.");

	window->LockMouseToWindow(true);
	window->ShowOSPointer(false);
}

void Game::LoadLevel(string levelFolder)
{
	//Create a new level and get all its assets in memory
	currentLevel = new Level(database);
	currentLevel->LoadAndInitialise(levelFolder);

	//Get rid of the loading screen and render the level
	database->GRenderer->Find("Renderer")->InitialiseScene();

	//Temporary singular poll for players 
	//when scene is being loaded
	ScoreBoard::Initialise(renderer);
	InitialisePlayers();
}

void Game::ExitLevel()
{
	if (currentLevel != nullptr) {
		inputManager->ClearAll();
		currentLevel->UnloadLevel();
		delete currentLevel;
	}
	else Log::Error("Trying to unload a level that has not been loaded!");
}

void Game::LoadNewLevel(string level)
{
	//Create a new level and get all its assets in memory
	currentLevel = new Level(database);
	currentLevel->LoadAndInitialise(level);

	//Get rid of the loading screen and render the level
	database->GRenderer->Find("Renderer")->InitialiseScene();

	//Temporary singular poll for players 
	//when scene is being loaded
	ScoreBoard::Initialise(renderer);
	inputManager->ReInitialisePlayers();
}

void Game::InitProfilerTimers()
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

void Game::InitialisePlayers()
{
	inputManager->ConnectToDataBase(database);
	inputManager->ConnectedGamePads(false);
}
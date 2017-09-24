#include "LevelLoader.h"
#include "Level.h"
#include "ScoreBoard.h"
#include "../ResourceManagment/DataBase.h"
#include "../UISystem/InputManager.h"

LevelLoader::LevelLoader(DataBase* database)
{
	this->database = database;
}

LevelLoader::LevelLoader()
{
	database = nullptr;
}

void LevelLoader::LoadFirstLevel(std::string level)
{
	LoadLevel(level);

	InputManager* inputManager =
		static_cast<InputManager*>(database->GetTable("GInputManager")->GetResources()->Find("InputManager"));

	inputManager->GetPlayerbase()->RetrieveGamepadsAndPlayers(database);
	inputManager->GetPlayerbase()->ConnectGamepads(false);
}

void LevelLoader::LoadReplacementLevel(std::string level)
{
	LoadLevel(level);

	InputManager* inputManager =
		static_cast<InputManager*>(database->GetTable("GInputManager")->GetResources()->Find("InputManager"));

	inputManager->GetPlayerbase()->ReInitialisePlayers();
}

void LevelLoader::ExitLevel()
{
	if (currentLevel != nullptr)
	{
		InputManager* inputManager =
			static_cast<InputManager*>(database->GetTable("GInputManager")->GetResources()->Find("InputManager"));

		inputManager->GetPlayerbase()->ClearAll();
		currentLevel->UnloadLevel();
		delete currentLevel;
	}
	else Log::Error("Trying to unload a level that has not been loaded!");
}

void LevelLoader::LoadLevel(std::string level)
{
	Renderer* renderer = static_cast<Renderer*>(database->GetTable("GRenderer")->GetResources()->Find("Renderer"));

	currentLevel = new Level(database);
	currentLevel->LoadAndInitialiseAssets(level);

	renderer->InitialiseScene();
	ScoreBoard::Initialise(renderer);
}
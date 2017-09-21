#include "LevelLoader.h"
#include "Level.h"
#include "ScoreBoard.h"
#include "../ResourceManagment/DataBase.h"

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

	database->GInputManager->Find("InputManager")->GetPlayerbase()->RetrieveGamepadsAndPlayers(database);
	database->GInputManager->Find("InputManager")->GetPlayerbase()->ConnectGamepads(false);
}

void LevelLoader::LoadReplacementLevel(std::string level)
{
	LoadLevel(level);

	database->GInputManager->Find("InputManager")->GetPlayerbase()->ReInitialisePlayers();
}

void LevelLoader::ExitLevel()
{
	if (currentLevel != nullptr)
	{
		database->GInputManager->Find("InputManager")->GetPlayerbase()->ClearAll();
		currentLevel->UnloadLevel();
		delete currentLevel;
	}
	else Log::Error("Trying to unload a level that has not been loaded!");
}

void LevelLoader::LoadLevel(std::string level)
{
	currentLevel = new Level(database);
	currentLevel->LoadAndInitialiseAssets(level);

	database->GRenderer->Find("Renderer")->InitialiseScene();
	ScoreBoard::Initialise(database->GRenderer->Find("Renderer"));
}
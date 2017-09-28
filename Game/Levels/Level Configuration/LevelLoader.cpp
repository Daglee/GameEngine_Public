#include "LevelLoader.h"
#include "../Level.h"
#include "../../Scoring/ScoreBoard.h"
#include "../ResourceManagment/Database/DataBase.h"
#include "../UISystem/InputManager.h"
#include "../nclgl/Rendering/Renderer.h"

LevelLoader::LevelLoader(DataBase* database)
{
	this->database = database;
	currentLevel = nullptr;
}

LevelLoader::LevelLoader()
{
	database = nullptr;
	currentLevel = nullptr;
}

void LevelLoader::LoadFirstLevel(const string level)
{
	LoadLevel(level);

	InputManager* inputManager =
		static_cast<InputManager*>(database->GetTable("GInputManager")->GetResources()->Find("InputManager"));

	inputManager->GetPlayerbase()->RetrieveGamepadsAndPlayers(database);
	inputManager->GetPlayerbase()->ConnectGamepads(false);
}

void LevelLoader::LoadReplacementLevel(const string level)
{
	LoadLevel(level);

	InputManager* inputManager =
		static_cast<InputManager*>(database->GetTable("GInputManager")->GetResources()->Find("InputManager"));

	inputManager->GetPlayerbase()->ReInitialisePlayers();
}

void LevelLoader::ExitLevel() const
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

void LevelLoader::LoadLevel(const string level)
{
	Renderer* renderer = static_cast<Renderer*>(database->GetTable("GRenderer")->GetResources()->Find("Renderer"));

	currentLevel = new Level(database);
	currentLevel->LoadAndInitialiseAssets(level);

	renderer->InitialiseScene();
	ScoreBoard::Initialise(renderer);
}
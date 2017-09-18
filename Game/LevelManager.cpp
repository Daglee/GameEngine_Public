#include "LevelManager.h"

#include "Launcher.h"
#include "../GameLogicFSM/MessageSystem.h"
#include "../ResourceManagment/Log.h"
#include "../ResourceManagment/DataBase.h"
#include "../nclgl/Renderer.h"

#define INITIAL_LEVEL 1
#define REPLACEMENT_LEVEL 2

LevelManager::LevelManager(DataBase* database, std::string filename) : FSMUnit("Levels", database)
{
	ConstructLevelList(filename);
	InitialiseFSM();
	database->GFSMManager->Find("GFSMManager")->LateBuildFSM("Levels", "../Data/GameLogic/Levels.txt");

	renderer = database->GRenderer->Find("Renderer");

	loader = LevelLoader(database);
}

void LevelManager::LoadFirstLevel()
{
	Update(0.0f);
}

void LevelManager::Update(const float& msec)
{
	//Is it time to stop this level?
	if (MessageSystem::GetInstance()->MessageTransmitting(Log::Hash("ExitLevel")))
	{
		MessageSystem::GetInstance()->StopTransmitting(Log::Hash("ExitLevel"));
		loader.ExitLevel();
	}

	//Should we load a new one?
	CheckLoadLevel();

	timer += msec / 1000.0f;

	//How much time is left for this level
	std::string mt = std::to_string((matchLength - timer));
	Text matchTimer(mt, Vector3((renderer->GetWidth() / 2) - 15, 0, 0), 15.0f, false);
	renderer->textbuffer.push_back(matchTimer);
}

void LevelManager::CheckLoadLevel()
{
	for (int i = 0; i < levelIDs.size(); ++i)
	{
		if (levelIDs[i] == INITIAL_LEVEL)
		{
			loader.LoadFirstLevel(levels[i]);
			levelIDs[i] = 0;
		}
		else if (levelIDs[i] == REPLACEMENT_LEVEL)
		{
			loader.LoadReplacementLevel(levels[i]);
			levelIDs[i] = 0;
		}
	}
}

void LevelManager::ConstructLevelList(std::string filename)
{
	std::ifstream file(filename);
	std::string line;

	int levelCount = 0;
	while (getline(file, line))
	{
		float id = 0;
		std::string levelDirectory = line;

		levels.push_back(levelDirectory);
		levelIDs.push_back(0);

		++levelCount;
	}
}

void LevelManager::InitialiseFSM()
{
	for (int i = 0; i < levelIDs.size(); ++i)
	{
		vars->insert({ "arena" + std::to_string(i + 1), &levelIDs[i] });
	}

	vars->insert({ "timer", &timer });
	vars->insert({ "matchLength", &matchLength });
}
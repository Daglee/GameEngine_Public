#include "LevelManager.h"

#include "../GameLogicFSM/MessageSystem.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../ResourceManagment/Log.h"
#include "../ResourceManagment/DataBase.h"
#include "../nclgl/Renderer.h"

const int INITIAL_LEVEL = 1;
const int REPLACEMENT_LEVEL = 2;

LevelManager::LevelManager(DataBase* database, const string filename) : FSMUnit("Levels", database)
{
	ConstructLevelList(filename);
	InitialiseFSM();

	FSMManager* fsmManager = static_cast<FSMManager*>(database->GetTable("GFSMManager")->GetResources()
		->Find("GFSMManager"));

	fsmManager->LateBuildFSM("Levels", "../Data/GameLogic/Levels.txt");

	renderer = static_cast<Renderer*>(database->GetTable("GRenderer")->GetResources()->Find("Renderer"));

	loader = LevelLoader(database);
}

void LevelManager::LoadFirstLevel()
{
	Update(0.0f);
}

void LevelManager::Update(const float& msec)
{
	if (MessageSystem::GetInstance()->MessageTransmitting(Log::Hash("ExitLevel")))
	{
		MessageSystem::GetInstance()->StopEvent(Log::Hash("ExitLevel"));
		loader.ExitLevel();
	}

	CheckLoadLevel();

	timer += msec / 1000.0f;

	const string timeLeft = to_string((matchLength - timer));
	const Text matchTimer(timeLeft, Vector3((renderer->GetWidth() / 2) - 15, 0, 0), 15.0f, false);

	renderer->AddText(matchTimer);
}

void LevelManager::CheckLoadLevel()
{
	for (unsigned i = 0; i < levelIDs.size(); ++i)
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

void LevelManager::ConstructLevelList(const string filename)
{
	ifstream file(filename);
	string line;

	int levelCount = 0;
	while (getline(file, line))
	{
		const string levelDirectory = line;

		levels.push_back(levelDirectory);
		levelIDs.push_back(0);

		++levelCount;
	}
}

void LevelManager::InitialiseFSM()
{
	for (int i = 0; i < levelIDs.size(); ++i)
	{
		properties->insert({ "arena" + to_string(i + 1), &levelIDs[i] });
	}

	properties->insert({ "timer", &timer });
	properties->insert({ "matchLength", &matchLength });
}
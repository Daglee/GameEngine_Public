#include "Level.h"
#include "../ResourceManagment/DataBase.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "../ResourceManagment/Log.h"
#include "../ResourceManagment/TemporaryDatabaseIdentifier.h"
#include "../ResourceManagment/GameEntityDatabaseEntryFile.h"
#include "LevelConfiguration.h"

#include <sstream>
#include <iostream>
#include <fstream>

Level::Level(DataBase* db)
{
	database = db;
}

void Level::LoadAndInitialiseAssets(std::string directory)
{
	//Directory file contains locations of the assets file and positions file
	std::ifstream file(directory);
	string path;

	getline(file, path);
	string assetsFile = path + "/assets.txt";
	string positionsFile = path + "/positions.txt";

	this->assetsfile = assetsFile; ////!!!!!!!!!!!!!!!!!
	LoadLevelAssets(assetsFile);
	InitialiseObjects(positionsFile);

	LevelConfiguration configuration(database, path);
	configuration.ConfigureFromAllExistingFiles();
}

void Level::LoadLevelAssets(std::string filename)
{
	std::ifstream file(filename);
	string line;

	getline(file, line);
	getline(file, line);

	while (getline(file, line))
	{
		vector<string> identifierTokens = Log::tokenise(line);
		ResourceIdentifier resourcesToAdd = ReadResourceIdentifier(identifierTokens);

		if (resourcesToAdd.numberInDatabase == 1)
		{
			database->AddToResourceManager(resourcesToAdd.resourceManager, resourcesToAdd.resourceName);
		}
		else
		{
			AddAssetsAndAppendNumberToName(resourcesToAdd);
		}
	}

	file.close();
}

void Level::AddAssetsAndAppendNumberToName(ResourceIdentifier entry)
{
	for (int i = 0; i < entry.numberInDatabase; i++)
	{
		string nameWithAppendedNumber = entry.resourceName + std::to_string(i);
		database->AddToResourceManager(entry.resourceManager, nameWithAppendedNumber);
	}
}

void Level::InitialiseObjects(std::string filename)
{
	GameEntityDatabaseEntryFile newEntry(database);
	newEntry.FillDatabaseFromFile(filename);
}

void Level::UnloadLevel()
{
	std::ifstream file(assetsfile);
	string line;

	getline(file, line);
	getline(file, line);

	while (getline(file, line))
	{
		vector<string> identifierTokens = Log::tokenise(line);
		ResourceIdentifier entryToRemove = ReadResourceIdentifier(identifierTokens);

		if (entryToRemove.numberInDatabase == 1)
		{
			TemporaryDatabaseIdentifier item(entryToRemove.resourceManager, entryToRemove.resourceName);
			item.UnloadFromDatabase(database);
		}
		else
		{
			UnloadSetOfAssets(entryToRemove);
		}
	}

	file.close();
}

void Level::UnloadSetOfAssets(ResourceIdentifier entry)
{
	for (int i = 0; i < entry.numberInDatabase; i++)
	{
		string resourceNameAndNumber = entry.resourceName + std::to_string(i);

		TemporaryDatabaseIdentifier item(entry.resourceManager, resourceNameAndNumber);
		item.UnloadFromDatabase(database);
	}
}

ResourceIdentifier Level::ReadResourceIdentifier(std::vector<std::string> tokens)
{
	ResourceIdentifier resource;

	resource.numberInDatabase = atoi(tokens.at(0).c_str());
	resource.resourceManager = tokens.at(1);
	resource.resourceName = tokens.at(2);


	return resource;
}

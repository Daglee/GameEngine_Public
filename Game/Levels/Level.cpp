#include "Level.h"
#include "../ResourceManagment/DataBase.h"
#include "../ResourceManagment/Log.h"
#include "../ResourceManagment/TemporaryDatabaseIdentifier.h"
#include "../ResourceManagment/GameEntityDatabaseEntryFile.h"
#include "Level Configuration/LevelConfiguration.h"

#include <fstream>

Level::Level(DataBase* database)
{
	this->database = database;
}

void Level::LoadAndInitialiseAssets(const string directory)
{
	//Directory file contains locations of the assets file and positions file
	ifstream file(directory);
	string path;

	getline(file, path);
	const string assetsFile = path + "/assets.txt";
	const string positionsFile = path + "/positions.txt";

	this->assetsfile = assetsFile;
	LoadLevelAssets(assetsFile);
	InitialiseObjects(positionsFile);

	LevelConfiguration configuration(database, path);
	configuration.ConfigureFromAllExistingFiles();
}

void Level::LoadLevelAssets(const string filename)
{
	ifstream file(filename);
	string line;

	getline(file, line);
	getline(file, line);

	while (getline(file, line))
	{
		const vector<string> identifierTokens = Log::tokenise(line);
		const ResourceIdentifier resourcesToAdd = ReadResourceIdentifier(identifierTokens);

		if (resourcesToAdd.numberInDatabase == 1)
		{
			database->AddResourceToTable(resourcesToAdd.resourceManager, resourcesToAdd.resourceName);
		}
		else
		{
			AddAssetsAndAppendNumberToName(resourcesToAdd);
		}
	}

	file.close();
}

void Level::AddAssetsAndAppendNumberToName(const ResourceIdentifier entry) const
{
	for (int i = 0; i < entry.numberInDatabase; i++)
	{
		const string nameWithAppendedNumber = entry.resourceName + to_string(i);
		database->AddResourceToTable(entry.resourceManager, nameWithAppendedNumber);
	}
}

void Level::InitialiseObjects(const string filename) const
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
		const vector<string> identifierTokens = Log::tokenise(line);
		const ResourceIdentifier entryToRemove = ReadResourceIdentifier(identifierTokens);

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

void Level::UnloadSetOfAssets(const ResourceIdentifier entry) const
{
	for (int i = 0; i < entry.numberInDatabase; i++)
	{
		const string resourceNameAndNumber = entry.resourceName + std::to_string(i);

		TemporaryDatabaseIdentifier item(entry.resourceManager, resourceNameAndNumber);
		item.UnloadFromDatabase(database);
	}
}

ResourceIdentifier Level::ReadResourceIdentifier(vector<string> tokens)
{
	ResourceIdentifier resource;

	resource.numberInDatabase = atoi(tokens.at(0).c_str());
	resource.resourceManager = tokens.at(1);
	resource.resourceName = tokens.at(2);


	return resource;
}

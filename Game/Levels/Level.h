#pragma once

#include <string>
#include <vector>

class DataBase;
class GameObject;

struct ResourceIdentifier
{
	int numberInDatabase;
	std::string resourceManager;
	std::string resourceName;
};

/*
  A Level will read in data from given text files to form assets.
*/
class Level
{
public:
	explicit Level(DataBase* database);
	~Level() {}

	void LoadAndInitialiseAssets(const std::string directory);

	void UnloadLevel();
private:
	void LoadLevelAssets(const std::string filename);
	void InitialiseObjects(const std::string filename) const;

	void AddAssetsAndAppendNumberToName(const ResourceIdentifier entry) const;
	void UnloadSetOfAssets(const ResourceIdentifier entry) const;

	ResourceIdentifier ReadResourceIdentifier(std::vector<std::string> tokens);

	DataBase* database;
	std::string assetsfile;
};


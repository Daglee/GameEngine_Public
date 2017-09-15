#pragma once

#include <string>
#include <vector>
#include <mutex>

class DataBase;
class GameObject;

struct ResourceIdentifier
{
	int numberInDatabase;
	std::string resourceManager;
	std::string resourceName;
};

/*
  A Level will read in data from given text files to:
   - Add any assets not already in the database (these may be specific to this level)
   - Set placeholder positions for these assets
   - Set parameters for these assets
  Loading and unloading assets for a map is done here.
*/
class Level
{
public:
	Level(DataBase* db);
	~Level() {}

	/*
	  Location of a file which contains the paths of the:
	   - assets file (what should be loaded into database)
	   - positions file (where should each asset be placed for now)
	   These are sent to seperate functions.
	*/
	void LoadAndInitialiseAssets(std::string directory);

	void UnloadLevel();
private:
	void LoadLevelAssets(std::string filename);
	void InitialiseObjects(std::string filename);

	void AddAssetsAndAppendNumberToName(ResourceIdentifier entry);
	void UnloadSetOfAssets(ResourceIdentifier entry);

	ResourceIdentifier ReadResourceIdentifier(std::vector<std::string> tokens);

	DataBase* database;
	std::string assetsfile; //Stored for easier unloading
};


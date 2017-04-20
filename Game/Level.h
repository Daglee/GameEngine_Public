#pragma once

#include <string>
#include <vector>

class DataBase;
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
	~Level(){}

	/*
	  Location of a file which contains the paths of the:
	   - assets file (what should be loaded into database)
	   - positions file (where should each asset be placed for now)
	   These are sent to seperate functions.
	*/
	void LoadAndInitialise(std::string directory);

	/*
	  Remove assets from the database after deleting
	  any objects placed in the level.
	*/
	void UnloadLevel();

	/*
	  Used to split reading the files into two seperate tasks
	  for threading. 
	*/
	void ReadLoop(std::vector<std::string> line, int numLines);

private:
	//Loads the required assets to the resource manager
	void LoadLevel(std::string filename);

	//Remove a single item from the database
	void UnloadItem(std::string resourcemanager, std::string resourcename);

	//Moves and orientates the objects appropriately
	void InitialiseObjects(std::string filename);

	//Build the FSMs required for this level
	void InitialiseGameLogic(std::string filename);

	//Add any background music needed
	void InitialiseMusic(std::string filename);

	//Reads a singular object
	void ReadObject(std::string line);

	void ReadPhysicsObject(std::string file);

	DataBase* database;
	std::string assetsfile; //Stored for easier unloading
};


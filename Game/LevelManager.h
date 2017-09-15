#pragma once

#include "../GameLogicFSM/FSMUnit.h"
#include "LevelLoader.h"

#include <string>
#include <map>
#include <vector>

class Launcher;
class DataBase;
class Renderer;

/*
  Uses a FSM to manage level transitions.
   - Based on messages being transmitted.
*/
class LevelManager : public FSMUnit
{
public:
	LevelManager(DataBase* database, std::string filename);
	~LevelManager();

	void LoadFirstLevel();

	void Update(const float& msec);
private:
	void CheckLoadLevel();

	void ConstructLevelList(std::string filename);
	void InitialiseFSM();

	Renderer*	renderer;
	LevelLoader loader;

	std::vector<float>			levelIDs;
	std::vector<std::string>	levels;

	float timer;
	float matchLength;
};


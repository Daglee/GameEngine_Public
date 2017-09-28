#pragma once

#include "../GameLogicFSM/FSMUnit.h"
#include "LevelLoader.h"

#include <string>
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
	LevelManager(DataBase* database, const string filename);
	~LevelManager() {}

	void LoadFirstLevel();

	void Update(const float& msec);

private:
	void CheckLoadLevel();

	void ConstructLevelList(const string filename);
	void InitialiseFSM();

	Renderer* renderer;
	LevelLoader loader;

	vector<float> levelIDs;
	vector<string> levels;

	float timer;
	float matchLength;
};


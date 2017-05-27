#pragma once

#include "../GameLogicFSM/FSMUnit.h"

#include <string>
#include <map>
#include <vector>

class Game;
class DataBase;
class Renderer;

/*
  Uses a FSM to manage level transitions.
   - Based on messages being transmitted.
*/
class LevelManager : public FSMUnit
{
public:
	LevelManager(Game* game, DataBase* database, std::string filename);
	~LevelManager();

	//Decide what to do.
	void Update(const float& msec);

	//Which level to load, if any.
	void CheckLoadLevel();

	//Whats the order of levels and which ones should be prepared.
	void ConstructLevelList(std::string filename);
	void InitialiseFSM();

	Game*		game;
	Renderer*	renderer;

	float timer;
	float matchLength;

	std::vector<float>			levelIDs;
	std::vector<std::string>	levels;
};


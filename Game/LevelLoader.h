#pragma once

#include <string>

class Level;
class DataBase;

class LevelLoader
{
public:
	LevelLoader(DataBase* database);
	LevelLoader();
	~LevelLoader() {}

	void LoadFirstLevel(std::string level);
	void LoadReplacementLevel(std::string level);
	void ExitLevel();

private:
	void LoadLevel(std::string level);

	Level* currentLevel;
	DataBase* database;
};


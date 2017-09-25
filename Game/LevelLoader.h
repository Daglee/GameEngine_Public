#pragma once

#include <string>

class Level;
class DataBase;

class LevelLoader
{
public:
	explicit LevelLoader(DataBase* database);
	LevelLoader();
	~LevelLoader() {}

	void LoadFirstLevel(const std::string level);
	void LoadReplacementLevel(const std::string level);
	void ExitLevel() const;

private:
	void LoadLevel(const std::string level);

	Level* currentLevel;
	DataBase* database;
};


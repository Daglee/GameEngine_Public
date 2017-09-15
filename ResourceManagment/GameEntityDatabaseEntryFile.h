#pragma once

#include <string>
#include <vector>

class DataBase;

class GameEntityDatabaseEntryFile
{
public:
	GameEntityDatabaseEntryFile(DataBase* database);
	~GameEntityDatabaseEntryFile() {}

	void FillDatabaseFromFile(std::string fileName);

private:
	void ReadAllObjects(std::vector<std::string> line, int numLines);
	void BuildEntity(std::string resourceManagerAndName);

	DataBase* database;
};


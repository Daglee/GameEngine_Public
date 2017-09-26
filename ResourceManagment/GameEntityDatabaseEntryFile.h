#pragma once

#include <string>
#include <vector>

class DataBase;

class GameEntityDatabaseEntryFile
{
public:
	GameEntityDatabaseEntryFile(DataBase* database);
	~GameEntityDatabaseEntryFile() {}

	void FillDatabaseFromFile(const std::string fileName);

private:
	void ReadAllObjects(std::vector<std::string> line, const int numLines) const;
	void BuildEntity(const std::string resourceManagerAndName) const;

	DataBase* database;
};


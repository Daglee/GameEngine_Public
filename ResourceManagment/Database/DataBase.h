#pragma once

#include "../Resources/Resource.h"
#include "Table.h"

#include <unordered_map>
#include <string>
#include <vector>

class DataBase
{
public:
	DataBase() {}
	~DataBase();

	void StartUp(string filename, bool rendererInitialised, bool renderLoadingScreen);

	void ReserveMemoryForAllTables() const;
	void AddResourceToTable(const string tableName, const string resourceName);

	void AddTable(const string tableName, Table<Resource>* table)
	{
		tables[tableName] = table;
	}

	Table<Resource>* GetTable(const string tableName)
	{
		return tables[tableName];
	}

	const size_t CurrentSize() const;
	const size_t MaxSize() const;

private:
	void ReadLoop(vector<vector<string>> tokens, vector<string> line, const int numLines);

	void ReadItem(vector<string> tokens);
	void ReadParameters(vector<string> tokens, string line,
		bool* renderInit, bool renderLoadingScreen);

	void ClearAllTables();

	unordered_map <string, Table<Resource>*> tables;
};


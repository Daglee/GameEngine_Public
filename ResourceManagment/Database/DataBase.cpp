#include "DataBase.h"

#include "../nclgl/Rendering/Renderer.h"
#include "../Threading/ThreadPool.h"

DataBase::~DataBase()
{
	ClearAllTables();
}

void DataBase::StartUp(string filename, bool rendererInitialised, bool renderLoadingScreen)
{
	ifstream file(filename);
	string line;

	bool renderInit = rendererInitialised;
	float loadCount = 0;

	getline(file, line); //Total number of items
	const float maxLoad = std::stof(line.c_str());

	getline(file, line); //Number of items to parse via threading
	const int threadCount = atoi(line.c_str());
	int mid = threadCount / 2;

	//Parsing lines that do not contain paramaters will be split into two threads
	//Thread0 will read from the start to the middle
	//Thread1 will read from the middle to the end at the same time
	int threadLine = 0;

	vector<vector<string>> tokensHolder0;
	vector<vector<string>> tokensHolder1;

	vector<string> lines1;
	vector<string> lines0;

	while (getline(file, line))
	{
		istringstream iss(line);

		vector<string> tokens{ std::istream_iterator<string>{iss},
			std::istream_iterator<string>{} };

		if (tokens.at(0) == "params")
		{
			ReadParameters(tokens, line, &renderInit, renderLoadingScreen);
		}
		else
		{
			if (threadLine < mid)
			{
				tokensHolder0.push_back(tokens);
				lines0.push_back(line);
			}
			else
			{
				tokensHolder1.push_back(tokens);
				lines1.push_back(line);
			}
			threadLine++;
		}

		loadCount++;
		if (renderInit && renderLoadingScreen)
		{
			Renderer* renderer = static_cast<Renderer*>(tables["GRenderer"]->GetResources()->Find("Renderer"));
			renderer->RenderLoadingScreen(loadCount, maxLoad);
		}
	}

	ThreadPool* threadPool = static_cast<ThreadPool*>(tables["GThreadPool"]->GetResources()->Find("ThreadPool"));

	std::vector<TaskFuture<void>> updates;

	updates.push_back(threadPool->SubmitJob([](
		DataBase& db, vector<vector<string>> tokenshold, vector<string> lines, int linenum)
	{
		db.ReadLoop(tokenshold, lines, linenum);
	}, std::ref(*this), tokensHolder0, lines0, mid));

	updates.push_back(threadPool->SubmitJob([](
		DataBase& db, vector<vector<string>> tokenshold, vector<string> lines, int linenum)
	{
		db.ReadLoop(tokenshold, lines, linenum);
	}, std::ref(*this), tokensHolder1, lines1, threadCount - mid));

	file.close();
}

void DataBase::ReserveMemoryForAllTables() const
{
	for each (auto table in tables)
	{
		table.second->ReserveInitialMemory();
	}
}

const size_t DataBase::CurrentSize() const
{
	size_t memory = 0;

	for each (auto table in tables)
	{
		memory += table.second->GetResources()->GetCurrentSize();
	}

	return memory;
}

const size_t DataBase::MaxSize() const
{
	size_t memory = 0;

	for each (auto table in tables)
	{
		memory += table.second->GetResources()->GetMaxSize();
	}

	return memory;
}

void DataBase::AddResourceToTable(const string tableName, const string resource)
{
	tables[tableName]->AddNewEntryFromFile(resource);
}

void DataBase::ReadLoop(vector<vector<string>> tokens, vector<string> line, const int numLines)
{
	for (int i = 0; i < numLines; i++)
	{
		ReadItem(tokens.at(i));
	}
}

void DataBase::ReadItem(vector<string> tokens)
{
	const int numOfItems = atoi(tokens.at(0).c_str());

	if (numOfItems == 1)
	{
		AddResourceToTable(tokens.at(1), tokens.at(2));
	}
	else
	{
		for (int i = 0; i < numOfItems; i++)
		{
			const string name = tokens.at(2) + to_string(i);
			AddResourceToTable(tokens.at(1), name);
		}
	}
}

void DataBase::ReadParameters(vector<string> tokens, string line, bool* renderInit, bool renderLoadingScreen)
{	
	//Remove parameters and type
	string params = line;

	params.erase(params.find(tokens.at(0)), tokens.at(0).size());
	params.erase(params.find(tokens.at(1)), tokens.at(1).size());
	AddResourceToTable(tokens.at(1), params);

	if (tokens.at(1) == "GRenderer" && *renderInit == false && renderLoadingScreen)
	{
		*renderInit = true; 
		
		Renderer* renderer = static_cast<Renderer*>(tables["GRenderer"]->GetResources()->Find("Renderer"));
		renderer->InitialiseLoadingScreen();
	}
}

void DataBase::ClearAllTables()
{
	for each (auto table in tables)
	{
		delete table.second;
	}

	tables.clear();
}

#include "DataBase.h"

#include "../nclgl/Renderer.h"
#include "../Threading/ThreadPool.h"

DataBase::~DataBase()
{
	ClearAllTables();
}

void DataBase::StartUp(std::string filename, bool rendererInitialised, bool renderLoadingScreen)
{
	std::ifstream file(filename);
	std::string line;

	bool renderInit = rendererInitialised;
	float loadCount = 0;

	getline(file, line); //Total number of items
	float maxLoad = std::stof(line.c_str());

	getline(file, line); //Number of items to parse via threading
	int threadCount = atoi(line.c_str());
	int mid = threadCount / 2;

	//Parsing lines that do not contain paramaters will be split into two threads
	//Thread0 will read from the start to the middle
	//Thread1 will read from the middle to the end at the same time
	int threadLine = 0;

	std::vector<std::vector<std::string>> tokensHolder0;
	std::vector<std::vector<std::string>> tokensHolder1;

	std::vector<std::string> lines1;
	std::vector<std::string> lines0;

	while (getline(file, line))
	{
		std::istringstream iss(line);

		std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>{} };

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

void DataBase::ReserveMemoryForAllTables()
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

void DataBase::AddResourceToTable(std::string tableName, std::string resource)
{
	tables[tableName]->AddNewEntryFromFile(resource);
}

void DataBase::ReadLoop(std::vector<std::vector<std::string>> tokens, std::vector<std::string> line, int numLines)
{
	for (int i = 0; i < numLines; i++)
	{
		ReadItem(tokens.at(i), line.at(i));
	}
}

void DataBase::ReadItem(std::vector<std::string> tokens, std::string line)
{
	int numOfItems = std::atoi(tokens.at(0).c_str());

	if (numOfItems == 1)
	{
		AddResourceToTable(tokens.at(1), tokens.at(2));
	}
	else
	{
		for (int i = 0; i < numOfItems; i++)
		{
			std::string name = tokens.at(2) + std::to_string(i);
			AddResourceToTable(tokens.at(1), name);
		}
	}
}

void DataBase::ReadParameters(std::vector<std::string> tokens, std::string line, bool* renderInit, bool renderLoadingScreen)
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

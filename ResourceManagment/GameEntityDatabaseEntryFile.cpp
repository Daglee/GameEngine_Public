#include "GameEntityDatabaseEntryFile.h"

#include "DataBase.h"
#include "../Threading/ThreadPool.h"
#include "../Game/GameEntityBuilder.h"
#include "../Game/PhysicsEntityBuilder.h"

#include <sstream>
#include <iostream>
#include <fstream>

GameEntityDatabaseEntryFile::GameEntityDatabaseEntryFile(DataBase* database)
{
	this->database = database;
}

void GameEntityDatabaseEntryFile::FillDatabaseFromFile(std::string fileName)
{
	std::ifstream file(fileName);
	string line;

	getline(file, line);
	int threadCount = atoi(line.c_str());	
	int middleOfFile = threadCount / 2;				

	int threadLine = 0;

	vector<string> firstHalf; //One thread for the first half of the file...
	vector<string> secondHalf; //And nother for the second half!

	while (getline(file, line)) {
		if (threadLine < middleOfFile) {
			firstHalf.push_back(line);
		}
		else {
			secondHalf.push_back(line);
		}
		threadLine++;
	}

	ThreadPool* threadPool = static_cast<ThreadPool*>(database->GetTable("GThreadPool")->GetResources()->Find("ThreadPool"));
	vector<TaskFuture<void>> threads;

	threads.push_back(threadPool->SubmitJob([](GameEntityDatabaseEntryFile& parser, vector<string> lines, int linenum) {
		parser.ReadAllObjects(lines, linenum);
	}, std::ref(*this), firstHalf, middleOfFile));

	threads.push_back(threadPool->SubmitJob([](GameEntityDatabaseEntryFile& parser, vector<string> lines, int linenum) {
		parser.ReadAllObjects(lines, linenum);
	}, std::ref(*this), secondHalf, threadCount - middleOfFile));

	for (auto& task : threads) {
		task.Complete();
	}

	file.close();
}

void GameEntityDatabaseEntryFile::ReadAllObjects(std::vector<std::string> line, int numLines)
{
	for (int i = 0; i < numLines; i++) {
		BuildEntity(line.at(i));
	}
}

void GameEntityDatabaseEntryFile::BuildEntity(std::string resourceManagerAndName)
{
	vector<string> tokens = Log::tokenise(resourceManagerAndName);

	string manager = tokens.at(0);

	if (manager == "PhysicsObjects") {
		PhysicsEntityBuilder physicsBuilder(database, tokens);
		physicsBuilder.Build();
	}
	else if (manager == "GameObjects") {
		GameEntityBuilder gameBuilder(database, tokens);
		gameBuilder.Build();
	}
}

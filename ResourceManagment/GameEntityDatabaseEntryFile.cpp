#include "GameEntityDatabaseEntryFile.h"

#include "DataBase.h"
#include "../Threading/ThreadPool.h"
#include "../Game/Asset Construction/GameEntityBuilder.h"
#include "../Game/Asset Construction/PhysicsEntityBuilder.h"

#include <fstream>

GameEntityDatabaseEntryFile::GameEntityDatabaseEntryFile(DataBase* database)
{
	this->database = database;
}

void GameEntityDatabaseEntryFile::FillDatabaseFromFile(const string fileName)
{
	ifstream file(fileName);
	string line;

	getline(file, line);
	const int threadCount = atoi(line.c_str());
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

void GameEntityDatabaseEntryFile::ReadAllObjects(vector<string> line, const int numLines) const
{
	for (int i = 0; i < numLines; i++) {
		BuildEntity(line.at(i));
	}
}

void GameEntityDatabaseEntryFile::BuildEntity(const string resourceManagerAndName) const
{
	vector<string> tokens = Log::tokenise(resourceManagerAndName);

	const string manager = tokens.at(0);

	if (manager == "PhysicsObjects") {
		PhysicsEntityBuilder physicsBuilder(database, tokens);
		physicsBuilder.Build();
	}
	else if (manager == "GameObjects") {
		GameEntityBuilder gameBuilder(database, tokens);
		gameBuilder.Build();
	}
}

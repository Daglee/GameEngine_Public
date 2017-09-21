#include "FSMManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../Game/Player.h"
#include "../Game/ScoreBoard.h"
#include "MessageSystem.h"

FSMManager::FSMManager(int numFSMs) : Resource()
{
	this->numFSMs = numFSMs;
	fsms = new FSM*[numFSMs];

	this->SetSize(sizeof(*this));
}


FSMManager::~FSMManager()
{
	for (int i = 0; i < numberOfFSMsAdded; ++i)
	{
		delete fsms[i];
	}

	delete fsms;
}

void FSMManager::LateBuildFSM(string FSMName, string filename)
{
	//Multiple FSMs could use the same config file.
	for each (FSM* fsm in fsmsNotBuilt)
	{
		if (fsm->FSMName == FSMName)
		{
			fsm->BuildFSM(filename);
		}
	}
}

void FSMManager::Update(float deltatime)
{
	updateTimer.StartTimer();

	this->SetSize(sizeof(*this) +
		sizeof(*MessageSystem::GetInstance()));

	//Update each FSM
	for (int i = 0; i < numberOfFSMsAdded; ++i)
	{
		fsms[i]->Update();
	}

	ScoreBoard::GetInstance()->DisplayScores();

	this->SetSize(sizeof(*this) +
		sizeof(*MessageSystem::GetInstance()));

	updateTimer.StopTimer();
}

void FSMManager::Read(string resourcename)
{
	this->SetName(resourcename);
}

void FSMManager::ReadParams(string params)
{
	std::istringstream iss(params);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	string name = tokens.at(0);
	int num = atoi(tokens.at(1).c_str());
	this->SetName(name);

	numFSMs = num;
	fsms = new FSM*[numFSMs];
	this->SetSize(sizeof(*this));
}
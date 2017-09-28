#include "FSMManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../Game/Players/Player.h"
#include "../Game/Scoring/ScoreBoard.h"
#include "MessageSystem.h"

FSMManager::FSMManager(const int numFSMs) : Resource()
{
	this->numFSMs = numFSMs;
	fsms = new FSM*[numFSMs];

	this->SetSizeInBytes(sizeof(*this));
}


FSMManager::~FSMManager()
{
	for (int i = 0; i < numberOfFSMsAdded; ++i)
	{
		delete fsms[i];
	}

	delete fsms;
}

void FSMManager::LateBuildFSM(string FSMName, string filename) const
{
	for each (FSM* fsm in fsmsNotBuilt)
	{
		if (fsm->FSMName == FSMName)
		{
			fsm->BuildFSM(filename);
		}
	}
}

void FSMManager::Update(const float& deltatime)
{
	updateTimer.StartTimer();

	this->SetSizeInBytes(sizeof(*this) +
		sizeof(*MessageSystem::GetInstance()));

	//Update each FSM
	for (int i = 0; i < numberOfFSMsAdded; ++i)
	{
		fsms[i]->Update();
	}

	ScoreBoard::GetInstance()->DisplayScores();

	this->SetSizeInBytes(sizeof(*this) +
		sizeof(*MessageSystem::GetInstance()));

	updateTimer.StopTimer();
}

void FSMManager::Read(const string resourcename)
{
	this->SetName(resourcename);
}

void FSMManager::ReadParams(const string params)
{
	std::istringstream iss(params);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	const string name = tokens.at(0);
	const int num = atoi(tokens.at(1).c_str());
	this->SetName(name);

	numFSMs = num;
	fsms = new FSM*[numFSMs];
	this->SetSizeInBytes(sizeof(*this));
}
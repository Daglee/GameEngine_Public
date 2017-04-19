#include "FSMManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../Game/Player.h"
#include "MessageSystem.h"

FSMManager::FSMManager(int numFSMs) : ResourceBase()
{
	this->numFSMs	= numFSMs;
	fsms			= new FSM*[numFSMs];
	//database		= db;

	this->SetResourceSize(sizeof(*this));
}


FSMManager::~FSMManager(){
	for (int i = 0; i < numAdded; ++i) 
	{
		delete fsms[i];
	}

	delete fsms;
}

void FSMManager::Update(float deltatime) 
{
	updateTimer.StartTimer();

	this->SetResourceSize(sizeof(*this) + 
		sizeof(*MessageSystem::GetInstance()));

	//Update each FSM
	for (int i = 0; i < numAdded; ++i)
	{
		fsms[i]->Update();
	}

	MessageSystem::GetInstance()->ClearAllMessages();

	this->SetResourceSize(sizeof(*this) + 
		sizeof(*MessageSystem::GetInstance()));

	updateTimer.StopTimer();
}

void FSMManager::Read(string resourcename)
{
	this->SetResourceName(resourcename);
}

void FSMManager::ReadParams(string params)
{
	std::istringstream iss(params);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	string name		= tokens.at(0);
	int num			= atoi(tokens.at(1).c_str());
	this->SetResourceName(name);
	
	numFSMs			= num;
	fsms = new FSM*[numFSMs];
	this->SetResourceSize(sizeof(*this));
}
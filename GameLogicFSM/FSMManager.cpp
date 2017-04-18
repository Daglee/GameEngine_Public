#include "FSMManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../Game/Player.h"
#include "MessageSystem.h"

FSMManager::FSMManager(DataBase* db, int numFSMs) : ResourceBase()
{
	this->numFSMs	= numFSMs;
	fsms			= new FSM*[numFSMs];
	database		= db;

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

	//Update each FSM
	for (int i = 0; i < numAdded; ++i)
	{
		fsms[i]->Update();
	}

	MessageSystem::GetInstance()->ClearAllMessages();
	updateTimer.StopTimer();
}

/*
  Iterate through the collider pairs and add to any game logic
  maps appropriately. 
*/
void  FSMManager::UpdateColliders() 
{
	for (std::map<RigidBody*, RigidBody*>::iterator i = colliders.begin();
		i != colliders.end(); i++) {
		string colliderA = i->first->tag;
		string colliderB = i->second->tag;
		
		string colliderAID = colliderA.substr(6, 1);
		string colliderBID = colliderB.substr(6, 1);

		if (colliderAID != colliderBID) {
			if (colliderA.find("bullet") != string::npos
				&& colliderB.find("player") != string::npos) {

				int playerID		= atoi(colliderBID.c_str()) - 1;
				string playername	= "player" + to_string(playerID);
				Player* player		= database->Players->Find(playername);

				float* h = new float(Log::Hash("bullet"));
				player->vars->find("collider")->second = h;
			}
			//Also check the other way round
			else if (colliderA.find("player") != string::npos
				&& colliderB.find("bullet") != string::npos) {

				int playerID		= atoi(colliderAID.c_str()) - 1;
				string playername	= "player" + to_string(playerID);
				Player* player		= database->Players->Find(playername);

				float* h = new float(Log::Hash("bullet"));
				player->vars->find("collider")->second = h;
			}
		}

	}

	colliders.clear();
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
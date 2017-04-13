#pragma once

#include "../ResourceManagment/ResourceBase.h"
#include "../Physics/RigidBody.h"
#include "../Game/Subsystem.h"
#include "FSM.h"
#include <map>

class DataBase;

/*
  Handles the game logic of the game via finite state machines.
  Main use is for colliders and how, if at all, to respond to them.

  Any class that will have a finite state machine acting on it must
  have a map of any modifiable variables - pair<string, float> 
  called "vars". Inheritance has been avoided to reduce overhead.
*/
class FSMManager : public ResourceBase, public Subsystem
{
public:
	FSMManager(DataBase* db, int numFSMs = 0);
	~FSMManager();

	void AddFSM(FSM* fsm) 
	{
		fsms[numAdded] = fsm;
		numAdded++;
	}

	void Update(float deltatime = 0); //For state changes
	void UpdateColliders();

	FSM** fsms;		//Array of FSMs
	int numFSMs;	//upper bounds of how many FSMs can be added
	int numAdded = 0;

	void Read(string resourcename);
	void ReadParams(string params);

	std::map<RigidBody*, RigidBody*> colliders; //Gets added to by the physics engine
private:
	DataBase* database;
};


#pragma once

#include "../ResourceManagment/Resource.h"
#include "../Physics/RigidBody.h"
#include "../Game/Subsystems/Subsystem.h"
#include "FSM.h"

/*
  Handles the game logic of the game via finite state machines.
*/
class FSMManager : public Resource, public Subsystem
{
public:
	explicit FSMManager(const int numFSMs = 0);
	~FSMManager();

	void AddFSM(FSM* fsm, const bool alreadyBuilt)
	{
		fsms[numberOfFSMsAdded] = fsm;

		if (!alreadyBuilt)
		{
			fsmsNotBuilt.push_back(fsm);
		}

		++numberOfFSMsAdded;
	}

	//Can be called to construct a FSM that has not been built yet.
	void LateBuildFSM(string FSMName, string filename) const;

	void Update(const float& deltatime = 0) override;

	FSM** fsms;				
	vector<FSM*> fsmsNotBuilt;	

	int numFSMs;	
	int numberOfFSMsAdded = 0;

	void Read(const string resourcename) override;
	void ReadParams(const string params) override;
};


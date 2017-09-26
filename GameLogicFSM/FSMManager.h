#pragma once

#include "../ResourceManagment/Resource.h"
#include "../Physics/RigidBody.h"
#include "../Game/Subsystem.h"
#include "FSM.h"

#include <map>

/*
  Handles the game logic of the game via finite state machines.
  Main use is for colliders and how, if at all, to respond to them.

  Any class that will have a finite state machine acting on it must
  have a map of any modifiable variables - pair<string, float>
  called "vars". Inheritance has been avoided to reduce overhead.
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

	void Update(const float& deltatime = 0) override; //For state changes

	FSM** fsms;				//Array of FSMs.
	vector<FSM*> fsmsNotBuilt;	//FSMs that have yet to be constructed.

	int numFSMs;	//upper bounds of how many FSMs can be added
	int numberOfFSMsAdded = 0;

	void Read(string resourcename) override;
	void ReadParams(string params) override;
};


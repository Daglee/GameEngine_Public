#pragma once

#include "State.h"
#include <fstream>

/*
  Finite state machine that contains states
  (which in turn have transitions attached to them).
  Can be updated to check transitions.
*/
class FSM
{
public:
	FSM(string FSMName, std::unordered_map<string, float*>* properties, string filename);
	FSM(string FSMName, std::unordered_map<string, float*>* properties);
	~FSM();

	//Read states and transitions.
	void BuildFSM(string newFileName = "");

	//State switching.
	void Update();

	vector<State*> states;

	//Retrieved from the object this FSM is created in.
	std::unordered_map<string, float*>* properties;

	string filename;
	string FSMName;

	//Each state has an ID. Default start state has ID 0.
	int currentActiveState = 0;
private:
	void ChangeToState(int nextStateIndex);

	void ReadStates(ifstream& file);
	void ReadActions(ifstream& file, State* state);
	void ReadTransitions(ifstream& file);
};


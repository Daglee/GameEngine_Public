#pragma once

#include "State.h"

/*
  Finite state machine that contains states
  (which in turn have transitions attached to them).
*/
class FSM
{
public:
	FSM(const string FSMName, unordered_map<string, float*>* properties, const string filename);
	FSM(const string FSMName, unordered_map<string, float*>* properties);
	~FSM();

	void BuildFSM(const string newFileName = "");

	//State switching.
	void Update();

	vector<State*> states;

	unordered_map<string, float*>* properties;

	string filename;
	string FSMName;

	int currentActiveState = 0;

private:
	void ChangeToState(const int nextStateIndex);

	void ReadStates(ifstream& file);
	void ReadActions(ifstream& file, State* state) const;
	void ReadTransitions(ifstream& file) const;
};


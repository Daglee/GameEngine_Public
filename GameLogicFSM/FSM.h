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
	FSM(std::unordered_map<string, float*>* vars, string filename);
	FSM(std::unordered_map<string, float*>* vars);
	~FSM();

	//Read states and transitions.
	void BuildFSM(string newFileName = "");	

	//State switching.
	void Update();		

	vector<State*> states;

	//Retrieved from the object this FSM is created in.
	std::unordered_map<string, float*>* vars;

	string filename;

	//Each state has an ID. Default start state has ID 0.
	int activeState = 0;
};


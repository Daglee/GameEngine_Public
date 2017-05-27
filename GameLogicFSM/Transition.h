#pragma once

#include "../ResourceManagment/Log.h"

#include <sstream>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

struct Check {
	vector<string> tokens;	//Just in case for any extra work needed.
	string	var;			//The tag of the variable it will modify.
	string	boolOperator;	//The condition.
	float	comparison;		//What we are comparing the variable to.
};

/*
  Dictates when the state of a finite state machine should change.
  Multiple of these could be attached and used. 
  FSM will read these in and construct them.
*/
class Transition
{
public:
	Transition(std::unordered_map<string, float*>* vars = NULL, int from = -1, 
		int to = -1, string check = "");

	~Transition() {}

	bool Check(); //Called by the FSM

	//Save the variables to save us from reading the string on each check
	void ConstructCheck();

	int to;				//State to move to
	int from;			//Starting state
	string checkstring;	//The condition itself, no parsing.
	//struct Check check;
	std::vector<struct Check> checks;

	//Obtained from the object that the FSM will act on
	std::unordered_map<string, float*>* vars; 
};


#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

using namespace std;

struct Check
{
	vector<string> tokens;	//Just in case for any extra work needed.
	string	property;			//The tag of the variable it will modify.
	string	boolOperator;	//The condition.
	float	comparison;		//What we are comparing the variable to.
	function<bool(Check*)> execute;
};

/*
  Dictates when the state of a finite state machine should change.
  Multiple of these could be attached and used.
  FSM will read these in and construct them.
*/
class Transition
{
public:
	Transition(unordered_map<string, float*>* properties = nullptr, const int startingState = -1,
		const int nextState = -1, const string check = "");

	~Transition() {}

	bool Check();

	void ConstructCheck();

	int nextState;		
	int startingState;	
	string checkstring;	
	vector<struct Check> checks;

	unordered_map<string, float*>* properties;
};


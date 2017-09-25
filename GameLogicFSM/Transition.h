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
	Transition(std::unordered_map<string, float*>* properties = nullptr, const int startingState = -1,
		const int nextState = -1, const string check = "");

	~Transition() {}

	bool Check(); //Called by the FSM

	//Save the variables to save us from reading the string on each check
	void ConstructCheck();

	int nextState;				//State to move to
	int startingState;			//Starting state
	string checkstring;	//The condition itself, no parsing.
	std::vector<struct Check> checks;

	//Obtained from the object that the FSM will act on
	std::unordered_map<string, float*>* properties;
};


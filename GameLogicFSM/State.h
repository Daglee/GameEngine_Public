#pragma once

#include "Transition.h"

#include <functional>

using namespace std;

struct Action
{
	vector<string> tokens;	//Just in case for any extra work needed.
	string	property;		//The tag of the variable it will modify.
	string	operatorName;	//The operator.
	float	operand;		//What we are going to set the variable to.
	std::function<void(Action*)> execute;
};

/*
  A state for a finite state machine!
  Will execute a series of actions once the state is set
  as active (in the finite state machine manager).
*/
class State
{
public:
	State(std::unordered_map<string, float*>* properties, int id = -1);

	virtual ~State() {}

	//Order could be important!
	virtual void ExecuteAllActions();

	//Save the variables to save us from reading the string on each check
	virtual void ConstructAction();

	int id;
	vector<string> actionNames;  //The actions without parsing
	vector<struct Action> actions; //To be executed on each update.

	//Obtained from the object that the FSM will act on
	std::unordered_map<string, float*>* properties;

	std::vector<Transition*> transitions;

	bool isTemp = false;
};


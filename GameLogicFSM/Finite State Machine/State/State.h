#pragma once

#include "../Transition/Transition.h"

#include <functional>

using namespace std;

struct Action
{
	vector<string> tokens;	//Just in case for any extra work needed.
	string	property;		//The tag of the variable it will modify.
	string	operatorName;	//The operator.
	float	operand;		//What we are going to set the variable to.
	function<void(Action*)> execute;
};

/*
  A state for a finite state machine!
  Will execute a series of actions once the state is set
  as active (in the finite state machine manager).
*/
class State
{
public:
	State(unordered_map<string, float*>* properties, const int id = -1);

	virtual ~State() {}

	virtual void ExecuteAllActions();
	virtual void ConstructAction();

	int id;
	vector<string> actionNames;
	vector<struct Action> actions;

	unordered_map<string, float*>* properties;

	vector<Transition*> transitions;

	bool isTemp = false;
};


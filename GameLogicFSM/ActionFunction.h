#pragma once

#include "State.h"

class ActionFunction
{
public:
	ActionFunction(unordered_map<string, float*>** properties, Action* action);
	~ActionFunction() {}

	void AssignExecution(const string operatorName);

private:
	unordered_map<string, function<void()>> assignments;

	void AssignEquals();
	void AssignMinus();
	void AssignPlus();
	void AssignMultiply();
	void AssignTransmit() const;
	void AssignNewEvent() const;
	void AssignEndEvent() const;
	void AssignAddPoints() const;

	Action* action;
	unordered_map<string, float*>** properties;
};


#pragma once

#include "State.h"

class ActionFunction
{
public:
	ActionFunction(std::unordered_map<string, float*>** properties, Action* action);
	~ActionFunction() {}

	void AssignExecution(std::string operatorName);

private:
	unordered_map<std::string, std::function<void()>> assignments;

	void AssignEquals();
	void AssignMinus();
	void AssignPlus();
	void AssignMultiply();
	void AssignTransmit();
	void AssignNewEvent();
	void AssignEndEvent();
	void AssignAddPoints();

	Action* action;
	std::unordered_map<string, float*>** properties;
};


#pragma once

#include "Transition.h"

class TransitionCheck
{
public:
	TransitionCheck(unordered_map<string, float*>** properties, Check* check);
	~TransitionCheck() {}

	void AssignCheck(const string operatorName);
private:
	void AssignEqual();
	void AssignNotEqual();
	void AssignGreaterThan();
	void AssignLessThan();
	void AssignExistsTransmission() const;
	void AssignDoesNotExistTransmission() const;

	Check* check;
	unordered_map<string, function<void()>> assignments;
	unordered_map<string, float*>** properties;
};


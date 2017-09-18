#pragma once

#include "Transition.h"

class TransitionCheck
{
public:
	TransitionCheck(std::unordered_map<string, float*>** properties, Check* check);
	~TransitionCheck() {}

	void AssignCheck(std::string operatorName);
private:
	void AssignEqual();
	void AssignNotEqual();
	void AssignGreaterThan();
	void AssignLessThan();
	void AssignExistsTransmission();
	void AssignDoesNotExistTransmission();

	Check* check;
	std::unordered_map<std::string, std::function<void()>> assignments;
	std::unordered_map<string, float*>** properties;
};


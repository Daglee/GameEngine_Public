#pragma once

#include "FSM.h"

#include <unordered_map>

class DataBase;

class FSMUnit
{
public:

	FSMUnit(std::string FSMName, DataBase* database);

	virtual ~FSMUnit() {}

	std::unordered_map<std::string, float*>* vars;
	FSM* fsm;
};


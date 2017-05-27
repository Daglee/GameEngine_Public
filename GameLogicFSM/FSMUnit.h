#pragma once

#include "FSM.h"

#include <unordered_map>

class DataBase;

/*
  All classes that will have a FSM to manage their 
  game logic must inherit from this.

  Makes use of late builds so that the level specifies
  the configuration for each FSM.
*/
class FSMUnit
{
public:

	FSMUnit(std::string FSMName = "", DataBase* database = nullptr);

	virtual ~FSMUnit() {}

	std::unordered_map<std::string, float*>* vars;
	FSM* fsm;
};


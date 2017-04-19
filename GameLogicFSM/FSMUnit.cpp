#include "FSMUnit.h"

#include "../ResourceManagment/DataBase.h"

FSMUnit::FSMUnit(std::string FSMName, DataBase* database)
{
	vars = new std::unordered_map<std::string, float*>;

	fsm = new FSM(vars);
}
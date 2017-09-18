#include "FSMUnit.h"

#include "../ResourceManagment/DataBase.h"

FSMUnit::FSMUnit(std::string FSMName, DataBase* database)
{
	properties = new std::unordered_map<std::string, float*>;
	fsm = new FSM(FSMName, properties);

	database->GFSMManager->Find("GFSMManager")->AddFSM(fsm, false);
}
#include "FSMUnit.h"

#include "../ResourceManagment/Database/DataBase.h"
#include "../../FSMManager.h"

FSMUnit::FSMUnit(const string FSMName, DataBase* database)
{
	properties = new std::unordered_map<std::string, float*>;
	fsm = new FSM(FSMName, properties);


	FSMManager* fsmm = static_cast<FSMManager*>(database->GetTable("GFSMManager")->GetResources()->Find("GFSMManager"));
	fsmm->AddFSM(fsm, false);
}
#include "State.h"
#include "../ResourceManagment/Log.h"
#include "MessageSystem.h"
#include "../Game/ScoreBoard.h"
#include "ActionFunction.h"

const string STRING_TYPE = "s:";

State::State(std::unordered_map<string, float*>* properties, const int id)
{
	this->id = id;
	this->properties = properties;
}

void State::ExecuteAllActions()
{
	for each (Action action in actions)
	{
		action.execute(&action);
	}
}

void State::ConstructAction()
{
	for each(string actionName in actionNames)
	{
		Action action;
		vector<string> tokens = Log::tokenise(actionName);

		action.property = tokens.at(0);
		action.operatorName = tokens.at(1);

		if (tokens.at(2).find(STRING_TYPE) != string::npos)
		{
			action.operand = Log::Hash(tokens.at(2).substr(2));
		}
		else
		{
			action.operand = stof(tokens.at(2).c_str());
		}

		ActionFunction functionAssignment(&properties, &action);
		functionAssignment.AssignExecution(action.operatorName);

		actions.push_back(action);
	}
}
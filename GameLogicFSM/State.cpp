#include "State.h"
#include "../ResourceManagment/Log.h"
#include "MessageSystem.h"

State::State(std::unordered_map<string, float*>* vars, int id)
{
	this->id = id;
	this->vars = vars;
}

//Change some variables in the object vars is in
void State::ExecuteAllActions()
{
	for each (Action a in actions) {
		if (a.actoperator == "=") {
			*vars->find(a.var)->second = a.action;
		}	
		else if (a.actoperator == "-=") {
			*vars->find(a.var)->second -= a.action;
		}
		else if (a.actoperator == "+=") {
			*vars->find(a.var)->second += a.action;
		}
		else if (a.actoperator == "*=") {
			*vars->find(a.var)->second *= a.action;
		}
		else if (a.actoperator == "transmit") {
			MessageSystem::GetInstance()->Transmit(a.action, false);
		}
	}
}

//Save the check to stop the stirng from being parsed
//every frame.
void State::ConstructAction()
{
	for each(string a in actionstrings) {
		Action act;
		vector<string> tokens = Log::tokenise(a);

		act.var = tokens.at(0);
		act.actoperator = tokens.at(1);

		if (tokens.at(2).find("s:") != string::npos) {
			act.action = Log::Hash(tokens.at(2).substr(2));
		}
		else act.action = stof(tokens.at(2).c_str());

		actions.push_back(act);
	}
}
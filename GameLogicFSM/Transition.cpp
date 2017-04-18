#include "Transition.h"
#include "MessageSystem.h"

Transition::Transition(std::unordered_map<string, float*>* vars, int from, int to, string check)
{
	this->vars	= vars;
	this->to	= to;
	this->from	= from;
	this->checkstring = check;

	ConstructCheck();
}

//Is it time to transition to another state?
bool Transition::Check() 
{
	if (check.boolOperator == "==") {
		if (*vars->find(check.var)->second == check.comparison) {
			return true;
		}
	}
	else if (check.boolOperator == "!=") {
		if (*vars->find(check.var)->second != check.comparison) {
			return true;
		}
	}
	else if (check.boolOperator == ">") {
		if (*vars->find(check.var)->second > check.comparison) {
			return true;
		}
	}
	else if (check.boolOperator == "<") {
		if (*vars->find(check.var)->second < check.comparison) {
			return true;
		}
	}
	else if (check.var == "exists_transmission") {
		if (MessageSystem::GetInstance()->MessageTransmitting(check.comparison)) {
			return true;
		}
	}
	else return false;
}

//Save the check to stop the stirng from being parsed
//every frame.
void Transition::ConstructCheck()
{
	vector<string> tokens = Log::tokenise(checkstring);

	check.var = tokens.at(0);
	check.boolOperator = tokens.at(1);

	/*
	  "s:" is a string that has to be changed or used.
	  Typically for collider tags.
	  It's just hashed and put in the map.
	*/
	if (tokens.at(2).find("s:") != string::npos) {
		string substring = tokens.at(2).substr(2);
		check.comparison = Log::Hash(substring);
	}
	else check.comparison = stof(tokens.at(2).c_str());
}
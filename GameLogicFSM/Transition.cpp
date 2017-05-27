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
	vector<bool> results;

	for each (struct Check check in checks) {
		if (check.boolOperator == "==") {
			if (*vars->find(check.var)->second == check.comparison) results.push_back(true);
			else results.push_back(false);
		}
		else if (check.boolOperator == "!=") {
			if (*vars->find(check.var)->second != check.comparison) results.push_back(true);
			else results.push_back(false);
		}
		else if (check.boolOperator == ">") {
			if (*vars->find(check.var)->second > check.comparison) results.push_back(true);
			else results.push_back(false);
		}
		else if (check.boolOperator == "<") {
			if (*vars->find(check.var)->second < check.comparison) results.push_back(true);
			else results.push_back(false);
		}
		else if (check.var == "exists_transmission") {
			if (MessageSystem::GetInstance()->MessageTransmitting(check.comparison)) results.push_back(true);
			else results.push_back(false);
		}
		else if (check.var == "!exists_transmission") {
			if (!MessageSystem::GetInstance()->MessageTransmitting(check.comparison)) results.push_back(true);
			else results.push_back(false);
		}
		else results.push_back(false);
	}

	bool finalBool = false || results[0];

	for (int i = 1; i < results.size(); ++i) {
		finalBool = finalBool && results[i];
	}

	return finalBool;
}

//Save the check to stop the stirng from being parsed
//every frame.
void Transition::ConstructCheck()
{
	vector<string> tokens = Log::tokenise(checkstring);

	int wordCount = -1;
	for (int i = 0; i < tokens.size(); i += 4) {
		struct Check check;

		check.var = tokens.at(i);
		check.boolOperator = tokens.at(i + 1);

		/*
		"s:" is a string that has to be changed or used.
		Typically for collider tags.
		It's just hashed and put in the map.
		*/
		if (tokens.at(i + 2).find("s:") != string::npos) {
			string substring = tokens.at(i + 2).substr(2);
			check.comparison = Log::Hash(substring);
		}
		else check.comparison = stof(tokens.at(i + 2).c_str());

		checks.push_back(check);

		wordCount = -1;
	}
}
#include "Transition.h"

#include "MessageSystem.h"
#include "TransitionCheck.h"

Transition::Transition(std::unordered_map<string, float*>* properties, int startingState, int nextState, string check)
{
	this->properties = properties;
	this->nextState = nextState;
	this->startingState = startingState;
	this->checkstring = check;

	ConstructCheck();
}

//Is it time to transition to another state?
bool Transition::Check() 
{
	vector<bool> results;

	for each (struct Check check in checks) {
		bool transition = check.execute(&check);
		results.push_back(transition);
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

		check.property = tokens.at(i);
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

		TransitionCheck checkAssignment(&properties, &check);

		if (check.property == "exists_transmission" || check.property == "!exists_transmission")
		{
			checkAssignment.AssignCheck(check.property);
		}
		else
		{
			checkAssignment.AssignCheck(check.boolOperator);
		}

		checks.push_back(check);

		wordCount = -1;
	}
}
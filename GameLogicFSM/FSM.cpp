#include "FSM.h"

#include "../ResourceManagment/Log.h"

FSM::FSM(std::unordered_map<string, float*>* vars, string filename)
{
	this->vars = vars;
	this->filename = filename;
	BuildFSM();
}

FSM::~FSM()
{
	for each(State* state in states) {
		delete state;
	}
}

void FSM::BuildFSM() 
{
	std::ifstream file(filename);
	string line;
	
	getline(file, line);
	int numStates = atoi(line.c_str());

	getline(file, line);
	while (line != "-") {
		vector<string> tokens = Log::tokenise(line);

		int id = atoi(tokens.at(1).c_str());
		State* s = new State(vars, id);

		getline(file, line);
		while (line.find("--") == string::npos) {
			string action = line;
			s->actionstrings.push_back(action);
			getline(file, line);
		}

		states.push_back(s);

		s->ConstructAction();

		getline(file, line);
	}

	getline(file, line);

	//Read Transitions
	while (line != "-") {

		getline(file, line);
		vector<string> tokens = Log::tokenise(line);

		int from = atoi(tokens.at(0).c_str());
		int to = atoi(tokens.at(2).c_str());

		getline(file, line);
		string check = line;

		Transition* t = new Transition(vars, from, to, check);

		states[from]->transition = t;

		getline(file, line);
	}

	states[activeState]->ExecuteAllActions();
}

void FSM::Update() 
{
	//If transition returns true, move to a new state
	if (states[activeState]->transition->Check()) {
		activeState = states[activeState]->transition->to;
		states[activeState]->ExecuteAllActions();
	}
	//else no change
}
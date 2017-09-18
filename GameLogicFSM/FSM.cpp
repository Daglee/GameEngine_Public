#include "FSM.h"

#include "../ResourceManagment/Log.h"

const int PERMANENT_STATE_LINE_LENGTH = 2;
const string ACTION_DELIMITER = "--";
const string STATE_DELIMITER = "-";

FSM::FSM(string FSMName, std::unordered_map<string, float*>* properties, string filename)
{
	this->properties = properties;
	this->filename = filename;
	this->FSMName = FSMName;

	BuildFSM();
}

FSM::FSM(string FSMName, std::unordered_map<string, float*>* properties)
{
	this->properties = properties;
	this->FSMName = FSMName;
}

FSM::~FSM()
{
	for each(State* state in states)
	{
		delete state;
	}
}

void FSM::BuildFSM(string newFileName)
{
	if (newFileName != "")
	{
		filename = newFileName;
	}

	std::ifstream file(filename);
	string line;

	getline(file, line);
	int numStates = atoi(line.c_str());

	ReadStates(file);
	ReadTransitions(file);

	states[currentActiveState]->ExecuteAllActions();
}

void FSM::Update()
{
	//If transition returns true, move to a new state
	for (int i = 0; i < states[currentActiveState]->transitions.size(); ++i)
	{
		Transition* currentTransition = states[currentActiveState]->transitions[i];

		if (currentTransition->Check())
		{
			ChangeToState(currentTransition->nextState);
			break;
		}
	}
}

void FSM::ChangeToState(int nextStateIndex)
{
	State* nextState = states[nextStateIndex];

	if (nextState->isTemp)
	{
		//If a temporary state, execute the actions but dont change state.
		nextState->ExecuteAllActions();
	}
	else
	{
		//The new State is the one that the transition specifies.
		currentActiveState = nextStateIndex;
		states[currentActiveState]->ExecuteAllActions();
	}
}

void FSM::ReadStates(ifstream& file)
{
	string line;
	getline(file, line);

	while (line != STATE_DELIMITER)
	{
		vector<string> tokens = Log::tokenise(line);

		int stateID = atoi(tokens.at(1).c_str());

		State* state = new State(properties, stateID);

		if (tokens.size() > PERMANENT_STATE_LINE_LENGTH)
		{
			state->isTemp = true;
		}

		ReadActions(file, state);

		states.push_back(state);
		state->ConstructAction();

		getline(file, line);
	}
}

void FSM::ReadActions(ifstream& file, State* state)
{
	string line;
	getline(file, line);

	while (line.find(ACTION_DELIMITER) == string::npos)
	{
		string action = line;
		state->actionNames.push_back(action);

		getline(file, line);
	}
}

void FSM::ReadTransitions(ifstream& file)
{
	string line;
	getline(file, line);

	while (line != STATE_DELIMITER)
	{
		getline(file, line);
		vector<string> tokens = Log::tokenise(line);

		int startingState = atoi(tokens.at(0).c_str());
		int stateToMoveTo = atoi(tokens.at(2).c_str());

		getline(file, line);
		string check = line;

		Transition* transition = new Transition(properties, startingState, stateToMoveTo, check);
		states[startingState]->transitions.push_back(transition);

		getline(file, line);
	}
}

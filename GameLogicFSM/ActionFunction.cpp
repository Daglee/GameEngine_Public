#include "ActionFunction.h"

#include "MessageSystem.h"
#include "../Game/ScoreBoard.h"

ActionFunction::ActionFunction(std::unordered_map<string, float*>** properties, Action* action)
{
	this->properties = properties;
	this->action = action;

	assignments.insert({ "=", std::bind(&ActionFunction::AssignEquals, this)});
	assignments.insert({ "-=", std::bind(&ActionFunction::AssignMinus, this) });
	assignments.insert({ "+=", std::bind(&ActionFunction::AssignPlus, this) });
	assignments.insert({ "*=", std::bind(&ActionFunction::AssignMultiply, this) });
	assignments.insert({ "transmit", std::bind(&ActionFunction::AssignTransmit, this) });
	assignments.insert({ "newevent", std::bind(&ActionFunction::AssignNewEvent, this) });
	assignments.insert({ "endevent", std::bind(&ActionFunction::AssignEndEvent, this) });
	assignments.insert({ "addpoints", std::bind(&ActionFunction::AssignAddPoints, this) });
}

void ActionFunction::AssignExecution(std::string operatorName)
{
	std::unordered_map<std::string, std::function<void()>>::const_iterator iterator = 
		assignments.find(operatorName);

	if (iterator != assignments.end())
	{
		iterator->second();
	}
}

void ActionFunction::AssignEquals()
{
	action->execute = [&propertiesMap = *properties](Action* action)
	{
		*propertiesMap->find(action->property)->second = action->operand;
	};
}

void ActionFunction::AssignMinus()
{
	action->execute = [&propertiesMap = *properties](Action* action)
	{
		*propertiesMap->find(action->property)->second -= action->operand;
	};
}

void ActionFunction::AssignPlus()
{
	action->execute = [&propertiesMap = *properties](Action* action)
	{
		*propertiesMap->find(action->property)->second += action->operand;
	};
}

void ActionFunction::AssignMultiply()
{
	action->execute = [&propertiesMap = *properties](Action* action)
	{
		*propertiesMap->find(action->property)->second *= action->operand;
	};
}

void ActionFunction::AssignTransmit()
{
	action->execute = [](Action* action)
	{
		MessageSystem::GetInstance()->TransmitMessage(action->operand);
	};
}

void ActionFunction::AssignNewEvent()
{
	action->execute = [](Action* action)
	{
		MessageSystem::GetInstance()->BeginEvent(action->operand);
	};
}

void ActionFunction::AssignEndEvent()
{
	action->execute = [](Action* action)
	{
		MessageSystem::GetInstance()->StopEvent(action->operand);
	};
}

void ActionFunction::AssignAddPoints()
{
	action->execute = [](Action* action)
	{
		ScoreBoard::GetInstance()->UpdateEntryScore(action->property, action->operand);
	};
}
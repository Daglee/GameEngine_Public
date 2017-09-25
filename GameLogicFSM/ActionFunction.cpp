#include "ActionFunction.h"

#include "MessageSystem.h"
#include "../Game/ScoreBoard.h"

ActionFunction::ActionFunction(unordered_map<string, float*>** properties, Action* action)
{
	this->properties = properties;
	this->action = action;

	assignments.insert({ "=", bind(&ActionFunction::AssignEquals, this)});
	assignments.insert({ "-=", bind(&ActionFunction::AssignMinus, this) });
	assignments.insert({ "+=", bind(&ActionFunction::AssignPlus, this) });
	assignments.insert({ "*=", bind(&ActionFunction::AssignMultiply, this) });
	assignments.insert({ "transmit", bind(&ActionFunction::AssignTransmit, this) });
	assignments.insert({ "newevent", bind(&ActionFunction::AssignNewEvent, this) });
	assignments.insert({ "endevent", bind(&ActionFunction::AssignEndEvent, this) });
	assignments.insert({ "addpoints", bind(&ActionFunction::AssignAddPoints, this) });
}

void ActionFunction::AssignExecution(const string operatorName)
{
	const unordered_map<string, function<void()>>::const_iterator iterator =
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

void ActionFunction::AssignTransmit() const
{
	action->execute = [](Action* action) 
	{
		MessageSystem::GetInstance()->TransmitMessage(action->operand);
	};
}

void ActionFunction::AssignNewEvent() const
{
	action->execute = [](Action* action)
	{
		MessageSystem::GetInstance()->BeginEvent(action->operand);
	};
}

void ActionFunction::AssignEndEvent() const
{
	action->execute = [](Action* action)
	{
		MessageSystem::GetInstance()->StopEvent(action->operand);
	};
}

void ActionFunction::AssignAddPoints() const
{
	action->execute = [](Action* action)
	{
		ScoreBoard::GetInstance()->UpdateEntryScore(action->property, action->operand);
	};
}
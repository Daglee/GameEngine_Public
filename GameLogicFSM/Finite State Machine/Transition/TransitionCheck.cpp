#include "TransitionCheck.h"

#include "../../Messaging/MessageSystem.h"

TransitionCheck::TransitionCheck(unordered_map<string, float*>** properties, Check* check)
{
	this->properties = properties;
	this->check = check;

	assignments.insert({ "==", std::bind(&TransitionCheck::AssignEqual, this) });
	assignments.insert({ "!=", std::bind(&TransitionCheck::AssignNotEqual, this) });
	assignments.insert({ ">", std::bind(&TransitionCheck::AssignGreaterThan, this) });
	assignments.insert({ "<", std::bind(&TransitionCheck::AssignLessThan, this) });
	assignments.insert({ "exists_transmission", std::bind(&TransitionCheck::AssignExistsTransmission, this) });
	assignments.insert({ "!exists_transmission", std::bind(&TransitionCheck::AssignDoesNotExistTransmission, this) });
}

void TransitionCheck::AssignCheck(const string operatorName)
{
	const unordered_map<std::string, std::function<void()>>::const_iterator iterator =
		assignments.find(operatorName);

	if (iterator != assignments.end())
	{
		iterator->second();
	}
}

void TransitionCheck::AssignEqual()
{
	check->execute = [&propertiesMap = *properties](Check* check)
	{
		return *propertiesMap->find(check->property)->second == check->comparison;
	};
}

void TransitionCheck::AssignNotEqual()
{
	
	check->execute = [&propertiesMap = *properties](Check* check)
	{
		return *propertiesMap->find(check->property)->second != check->comparison;
	};
}

void TransitionCheck::AssignGreaterThan()
{
	check->execute = [&propertiesMap = *properties](Check* check)
	{
		return *propertiesMap->find(check->property)->second > check->comparison;
	};
}

void TransitionCheck::AssignLessThan()
{
	check->execute = [&propertiesMap = *properties](Check* check)
	{
		return *propertiesMap->find(check->property)->second < check->comparison;
	};
}

void TransitionCheck::AssignExistsTransmission() const
{
	check->execute = [](Check* check)
	{
		return MessageSystem::GetInstance()->MessageTransmitting(check->comparison);
	};
}

void TransitionCheck::AssignDoesNotExistTransmission() const
{
	check->execute = [](Check* check)
	{
		return !MessageSystem::GetInstance()->MessageTransmitting(check->comparison);
	};
}

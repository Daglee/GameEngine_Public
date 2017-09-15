#pragma once

#include "EntityAttributeReader.h"

#include <functional>
#include <vector>

//class PhysicsObject;
class GameObject;

struct Property
{
	std::string name;
	std::function<void()> configurator;

	Property(std::string name, std::function<void()> configurator)
	{
		this->name = name;
		this->configurator = configurator;
	}
};

class EntityConfiguration
{
public:
	EntityConfiguration(std::vector<std::string> attributeTokens, GameObject* entity);

	~EntityConfiguration()
	{
		delete attributeReader;
	}

	void SetAttributeTokens(std::vector<std::string> newAttributeTokens)
	{
		attributeTokens = newAttributeTokens;
		attributeReader->SetTokens(newAttributeTokens);
	}

	void ConfigureEntityWithAttribute();
	bool IsFinished();
private:
	std::string currentAttribute;

	GameObject* entity;
	std::vector<std::string> attributeTokens;

	EntityAttributeReader* attributeReader;

	std::vector<Property> properties;
	std::vector<std::string> configurationTokens;
};


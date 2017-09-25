#pragma once

#include "EntityAttributeReader.h"

#include <functional>
#include <vector>

class GameObject;

struct Property
{
	std::string name;
	std::function<void()> configurator;

	Property(const std::string name, std::function<void()> configurator)
	{
		this->name = name;
		this->configurator = configurator;
	}
};

class EntityConfiguration
{
public:
	EntityConfiguration(const std::vector<std::string> attributeTokens, GameObject* entity);

	~EntityConfiguration()
	{
		delete attributeReader;
	}

	void SetAttributeTokens(const std::vector<std::string> newAttributeTokens)
	{
		attributeTokens = newAttributeTokens;
		attributeReader->SetTokens(newAttributeTokens);
	}

	void ConfigureEntityWithAttribute();
	const bool IsFinished() const;

private:
	std::string currentAttribute;

	GameObject* entity;
	std::vector<std::string> attributeTokens;

	EntityAttributeReader* attributeReader;

	std::vector<Property> properties;
	std::vector<std::string> configurationTokens;
};


#include "EntityConfiguration.h"
#include "EntityAttributeReader.h"

#include "PhysicsObject.h"

EntityConfiguration::EntityConfiguration(std::vector<std::string> attributeTokens, GameObject* entity)
{
	this->attributeTokens = attributeTokens;
	this->entity = entity;

	attributeReader = new EntityAttributeReader(attributeTokens, entity);

	properties.push_back(Property("size", std::bind(&EntityAttributeReader::SetSize, attributeReader)));
	properties.push_back(Property("colour", std::bind(&EntityAttributeReader::SetColour, attributeReader)));
	properties.push_back(Property("texture", std::bind(&EntityAttributeReader::SetTexture, attributeReader)));
	properties.push_back(Property("gravity", std::bind(&EntityAttributeReader::SetGravity, attributeReader)));
	properties.push_back(Property("mass", std::bind(&EntityAttributeReader::SetMass, attributeReader)));
	properties.push_back(Property("damping", std::bind(&EntityAttributeReader::SetDamping, attributeReader)));
	properties.push_back(Property("isMoveable", std::bind(&EntityAttributeReader::SetMoveable, attributeReader)));
	properties.push_back(Property("atRest", std::bind(&EntityAttributeReader::SetAtRest, attributeReader)));
	properties.push_back(Property("tag", std::bind(&EntityAttributeReader::SetTag, attributeReader)));
	properties.push_back(Property("rotation", std::bind(&EntityAttributeReader::SetRotation, attributeReader)));
}

void EntityConfiguration::ConfigureEntityWithAttribute()
{
	currentAttribute = attributeTokens.at(0);

	for each (Property entityProperty in properties)
	{
		if (currentAttribute == entityProperty.name)
		{
			entityProperty.configurator();
		}
	}
}

bool EntityConfiguration::IsFinished()
{
	return currentAttribute == "-";
}



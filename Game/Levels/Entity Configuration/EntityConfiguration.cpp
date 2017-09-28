#include "EntityConfiguration.h"
#include "EntityAttributeReader.h"

#include "../../Structure/PhysicsObject.h"

EntityConfiguration::EntityConfiguration(const vector<string> attributeTokens, GameObject* entity)
{
	this->attributeTokens = attributeTokens;
	this->entity = entity;

	attributeReader = new EntityAttributeReader(attributeTokens, entity);

	properties.push_back(Property("size", bind(&EntityAttributeReader::SetSize, attributeReader)));
	properties.push_back(Property("colour", bind(&EntityAttributeReader::SetColour, attributeReader)));
	properties.push_back(Property("texture", bind(&EntityAttributeReader::SetTexture, attributeReader)));
	properties.push_back(Property("gravity", bind(&EntityAttributeReader::SetGravity, attributeReader)));
	properties.push_back(Property("mass", bind(&EntityAttributeReader::SetMass, attributeReader)));
	properties.push_back(Property("damping", bind(&EntityAttributeReader::SetDamping, attributeReader)));
	properties.push_back(Property("isMoveable", bind(&EntityAttributeReader::SetMoveable, attributeReader)));
	properties.push_back(Property("atRest", bind(&EntityAttributeReader::SetAtRest, attributeReader)));
	properties.push_back(Property("tag", bind(&EntityAttributeReader::SetTag, attributeReader)));
	properties.push_back(Property("rotation", bind(&EntityAttributeReader::SetRotation, attributeReader)));
}

void EntityConfiguration::ConfigureEntityWithAttribute()
{
	attributeName = attributeTokens.at(0);

	for each (Property entityProperty in properties)
	{
		if (attributeName == entityProperty.name)
		{
			entityProperty.configurator();
		}
	}
}

const bool EntityConfiguration::IsFinished() const
{
	return attributeName == "-";
}



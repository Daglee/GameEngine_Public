#pragma once

#include <vector>
#include <string>

class GameObject;

class EntityAttributeReader
{
public:
	EntityAttributeReader(std::vector<std::string> tokens, GameObject* entity);
	~EntityAttributeReader();

	void SetTokens(std::vector<std::string> newTokens)
	{
		tokens = newTokens;
	}

	void SetEntity(GameObject* newEntity)
	{
		entity = newEntity;
	}

	void SetSize();
	void SetColour();
	void SetTexture();
	void SetGravity();
	void SetMass();
	void SetDamping();
	void SetMoveable();
	void SetAtRest();
	void SetTag();
	void SetRotation();

private:
	std::vector<std::string> tokens;
	GameObject* entity;
};


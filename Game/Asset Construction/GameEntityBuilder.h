#pragma once

#include <vector>
#include <string>

#include "../nclgl/Vector3.h"

class DataBase;
class GameObject;

class GameEntityBuilder
{
public:
	GameEntityBuilder(DataBase* database, const std::vector<std::string> configurationTokens);
	~GameEntityBuilder() {}

	void Build();

private:
	void StoreEntityFromDatabase();
	void AddStoredGameEntity() const;

	Vector3 ReadPosition() const;
	void AddMeshFromAssetName();

	GameObject* entity;
	DataBase* database;
	std::vector<std::string> configurationTokens;
};


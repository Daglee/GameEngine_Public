#pragma once

#include <vector>
#include <string>

#include "../nclgl/Vector3.h"

class DataBase;
class GameObject;

class GameEntityBuilder
{
public:
	GameEntityBuilder(DataBase* database, std::vector<std::string> configurationTokens);
	~GameEntityBuilder();

	void Build();

private:
	void StoreEntityFromDatabase();
	void AddStoredGameEntity();

	Vector3 ReadPosition();
	void AddMeshFromAssetName();

	GameObject* entity;
	DataBase* database;
	std::vector<std::string> configurationTokens;
};


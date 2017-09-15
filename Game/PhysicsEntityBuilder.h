#pragma once

#include <vector>
#include <string>

#include "../nclgl/Vector3.h"

class DataBase;
class PhysicsObject;

class PhysicsEntityBuilder
{
public:
	PhysicsEntityBuilder(DataBase* database, std::vector<std::string> configurationTokens);
	PhysicsEntityBuilder(DataBase* database);

	~PhysicsEntityBuilder() {}

	void SetConfigurationTokens(std::vector<std::string> newTokens)
	{
		configurationTokens = newTokens;
	}

	void Build();

private:
	void StoreEntityFromDatabase();
	void AddStoredPhysicsEntity();

	void ReadShapeAndBuild();

	void ConstructSphere();
	void ConstructPlane();

	void AddMeshFromAssetName(int meshToken);

	Vector3 ReadPosition();

	PhysicsObject* entity;
	DataBase* database;
	std::vector<std::string> configurationTokens;
};


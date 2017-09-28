#pragma once

#include <vector>
#include <string>

#include "../nclgl/Maths/Vector3.h"

class DataBase;
class PhysicsObject;

class PhysicsEntityBuilder
{
public:
	PhysicsEntityBuilder(DataBase* database, const std::vector<std::string> configurationTokens);
	PhysicsEntityBuilder(DataBase* database);

	~PhysicsEntityBuilder() {}

	void SetConfigurationTokens(const std::vector<std::string> newTokens)
	{
		configurationTokens = newTokens;
	}

	void Build();

private:
	void StoreEntityFromDatabase();
	void AddStoredPhysicsEntity() const;

	void ReadShapeAndBuild();

	void ConstructSphere() const;
	void ConstructPlane() const;

	void AddMeshFromAssetName(const int meshToken);

	Vector3 ReadPosition() const;

	PhysicsObject* entity;
	DataBase* database;
	std::vector<std::string> configurationTokens;
};


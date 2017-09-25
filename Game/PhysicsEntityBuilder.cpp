#include "PhysicsEntityBuilder.h"

#include "PhysicsObject.h"
#include "../ResourceManagment/DataBase.h"
#include "../Physics/PlaneCollider.h"

const int ASSET_NAME = 1;
const int X_POSITION = 2;
const int Y_POSITION = 3;
const int Z_POSITION = 4;
const int SPHERE_SETTING = 6;
const int X_PLANE = 7;
const int Y_PLANE = 8;
const int Z_PLANE = 9;
const int PLANE_MESH = 10;
const int SPHERE_MESH = 7;

PhysicsEntityBuilder::PhysicsEntityBuilder(DataBase* database, std::vector<std::string> configurationTokens)
{
	this->database = database;
	this->configurationTokens = configurationTokens;
}

PhysicsEntityBuilder::PhysicsEntityBuilder(DataBase* database)
{
	this->database = database;
}

void PhysicsEntityBuilder::Build()
{
	StoreEntityFromDatabase();
	AddStoredPhysicsEntity();

	ReadShapeAndBuild();
}

void PhysicsEntityBuilder::StoreEntityFromDatabase()
{
	std::string name = configurationTokens.at(ASSET_NAME);
	entity = static_cast<PhysicsObject*>(database->GetTable("PhysicsObjects")->GetResources()->Find(name));
}

void PhysicsEntityBuilder::AddStoredPhysicsEntity()
{
	entity->AddToPhysicsEngine(static_cast<PhysicsEngine*>(database->GetTable("PhysicsEngine")->GetResources()->Find("PhysicsEngine")));
	entity->AddToRenderer(*static_cast<Renderer*>(database->GetTable("GRenderer")->GetResources()->Find("Renderer")));
}

void PhysicsEntityBuilder::ReadShapeAndBuild()
{
	entity->isSphere = std::atoi(configurationTokens.at(SPHERE_SETTING).c_str()) != 0;

	if (!entity->isSphere)
	{
		ConstructPlane();
		AddMeshFromAssetName(PLANE_MESH);
	}
	else
	{
		ConstructSphere();
		AddMeshFromAssetName(SPHERE_MESH);
	}

	entity->SetPosition(ReadPosition());
}

void PhysicsEntityBuilder::ConstructPlane()
{
	float x = stof(configurationTokens.at(X_PLANE));
	float y = stof(configurationTokens.at(Y_PLANE));
	float z = stof(configurationTokens.at(Z_PLANE));

	Vector3 colliderPosition(x, y, z);
	entity->GetRigidBody()->collider = new PlaneCollider(colliderPosition);
}

void PhysicsEntityBuilder::ConstructSphere()
{
	entity->GetRigidBody()->collider = new SphereCollider(1);
}

void PhysicsEntityBuilder::AddMeshFromAssetName(int meshToken)
{
	std::string meshAssetName = configurationTokens.at(meshToken);

	OBJMesh* mesh = static_cast<OBJMesh*>(database->GetTable("OBJMeshes")->GetResources()->Find(meshAssetName));
	entity->AddMesh(*mesh);
}

Vector3 PhysicsEntityBuilder::ReadPosition()
{
	float xPosition = strtof(configurationTokens.at(X_POSITION).c_str(), 0);
	float yPosition = strtof(configurationTokens.at(Y_POSITION).c_str(), 0);
	float zPosition = strtof(configurationTokens.at(Z_POSITION).c_str(), 0);

	return Vector3(xPosition, yPosition, zPosition);
}
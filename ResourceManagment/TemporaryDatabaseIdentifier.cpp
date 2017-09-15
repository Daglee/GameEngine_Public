#include "TemporaryDatabaseIdentifier.h"

#include "DataBase.h"

TemporaryDatabaseIdentifier::TemporaryDatabaseIdentifier(std::string resourceManager, std::string resourceName)
{
	this->resourceManager = resourceManager;
	this->resourceName = resourceName;

	unloadLocked = false;
}

void TemporaryDatabaseIdentifier::UnloadFromDatabase(DataBase* database)
{
	if (!unloadLocked)
	{
		if (resourceManager == "OBJMeshes") {
			OBJMeshUnload(database);
		}
		else if (resourceManager == "PhysicsObjects") {
			PhysicsEntityUnload(database);
		}
		else if (resourceManager == "GameObjects") {
			GameEntityUnload(database);
		}

		unloadLocked = true;
	}
}

void TemporaryDatabaseIdentifier::OBJMeshUnload(DataBase* database)
{
	database->OBJMeshes->Unload(resourceName);
}

void TemporaryDatabaseIdentifier::GameEntityUnload(DataBase* database)
{
	SceneNode* node = database->GameObjects->Find(resourceName)->GetSceneNode();
	database->GRenderer->Find("Renderer")->RemoveSceneNode(node);

	database->GameObjects->Unload(resourceName);
}

void TemporaryDatabaseIdentifier::PhysicsEntityUnload(DataBase* database)
{
	Renderer* renderer = database->GRenderer->Find("Renderer");
	PhysicsEngine* physicsEngine = database->GPhysicsEngine->Find("PhysicsEngine");

	SceneNode* node = database->PhysicsObjects->Find(resourceName)->GetSceneNode();
	RigidBody* rigidBody = database->PhysicsObjects->Find(resourceName)->GetRigidBody();

	renderer->RemoveSceneNode(node);
	physicsEngine->RemoveRigidBody(rigidBody);

	database->PhysicsObjects->Unload(resourceName);
}

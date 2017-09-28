#include "GameEntityBuilder.h"

#include "GameObject.h"
#include "../ResourceManagment/DataBase.h"
#include "../nclgl/Renderer.h"

const int ASSET_NAME = 1;
const int X_POSITION = 2;
const int Y_POSITION = 3;
const int Z_POSITION = 4;
const int MESH = 5;

GameEntityBuilder::GameEntityBuilder(DataBase* database, const vector<string> configurationTokens)
{
	this->database = database;
	this->configurationTokens = configurationTokens;

	entity = nullptr;
}

void GameEntityBuilder::Build()
{
	StoreEntityFromDatabase();
	AddStoredGameEntity();

	entity->SetPosition(ReadPosition());

	AddMeshFromAssetName();
}

void GameEntityBuilder::StoreEntityFromDatabase()
{
	const string name = configurationTokens.at(ASSET_NAME);
	entity = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResources()->Find(name));
}

void GameEntityBuilder::AddStoredGameEntity() const
{
	Renderer* renderer = static_cast<Renderer*>(database->GetTable("GRenderer")->GetResources()->Find("Renderer"));

	entity->AddToRenderer(*renderer);
}

Vector3 GameEntityBuilder::ReadPosition() const
{
	const float xPosition = strtof(configurationTokens.at(X_POSITION).c_str(), 0);
	const float yPosition = strtof(configurationTokens.at(Y_POSITION).c_str(), 0);
	const float zPosition = strtof(configurationTokens.at(Z_POSITION).c_str(), 0);

	return Vector3(xPosition, yPosition, zPosition);
}

void GameEntityBuilder::AddMeshFromAssetName()
{
	OBJMesh* mesh = static_cast<OBJMesh*>(database->GetTable("OBJMeshes")->GetResources()->Find(configurationTokens.at(MESH)));

	entity->AddMesh(*mesh);
}

#include "GameEntityBuilder.h"

#include "GameObject.h"
#include "../ResourceManagment/DataBase.h"

const int ASSET_NAME = 1;
const int X_POSITION = 2;
const int Y_POSITION = 3;
const int Z_POSITION = 4;
const int MESH = 5;

GameEntityBuilder::GameEntityBuilder(DataBase* database, std::vector<std::string> configurationTokens)
{
	this->database = database;
	this->configurationTokens = configurationTokens;
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
	std::string name = configurationTokens.at(ASSET_NAME);
	entity = database->GameObjects->Find(name);
}

void GameEntityBuilder::AddStoredGameEntity()
{
	entity->AddToRenderer(*database->GRenderer->Find("Renderer"));
}

Vector3 GameEntityBuilder::ReadPosition()
{
	float xPosition = strtof(configurationTokens.at(X_POSITION).c_str(), 0);
	float yPosition = strtof(configurationTokens.at(Y_POSITION).c_str(), 0);
	float zPosition = strtof(configurationTokens.at(Z_POSITION).c_str(), 0);

	return Vector3(xPosition, yPosition, zPosition);
}

void GameEntityBuilder::AddMeshFromAssetName()
{
	entity->AddMesh(*database->OBJMeshes->Find(configurationTokens.at(MESH)));
}

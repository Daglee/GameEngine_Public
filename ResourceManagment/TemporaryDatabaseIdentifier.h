#pragma once

#include <string>

class DataBase;

class TemporaryDatabaseIdentifier
{
public:
	TemporaryDatabaseIdentifier(std::string resourceManager, std::string resourceName);
	~TemporaryDatabaseIdentifier() {}

	void UnloadFromDatabase(DataBase* database);

private:
	void OBJMeshUnload(DataBase* database);
	void GameEntityUnload(DataBase* database);
	void PhysicsEntityUnload(DataBase* database);

	std::string resourceManager;
	std::string resourceName;
	bool unloadLocked;
};


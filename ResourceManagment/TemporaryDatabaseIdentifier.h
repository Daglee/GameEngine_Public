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
	void OBJMeshUnload(DataBase* database) const;
	void GameEntityUnload(DataBase* database) const;
	void PhysicsEntityUnload(DataBase* database) const;

	std::string resourceManager;
	std::string resourceName;
	bool unloadLocked;
};


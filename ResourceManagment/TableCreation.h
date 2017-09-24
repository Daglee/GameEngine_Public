#pragma once

class DataBase;

#include <functional>
#include <vector>

class TableCreation
{
public:
	TableCreation(DataBase* database);
	~TableCreation();

	void AddTablesToDatabase();

private:
	void AddProfiler();
	void AddInputManager();
	void AddPhysicsEngine();
	void AddFSMManager();
	void AddCamera();
	void AddGamepads();
	void AddOBJMeshes();
	void AddMeshes();
	void AddPhysicsObjects();
	void AddGameObjects();
	void AddPlayers();
	void AddThreadPool();
	void AddWindow();
	void AddRenderer();
	void AddAudioManager();

	std::vector<std::function<void()>> tableAdditions;

	DataBase* database;
	int* playerIDCount;
};


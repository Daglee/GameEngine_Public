#pragma once

class DataBase;

#include <functional>
#include <vector>

class TableCreation
{
public:
	explicit TableCreation(DataBase* database);
	~TableCreation();

	void AddTablesToDatabase() const;

private:
	void AddProfiler() const;
	void AddInputManager() const;
	void AddPhysicsEngine() const;
	void AddFSMManager() const;
	void AddCamera() const;
	void AddGamepads() const;
	void AddOBJMeshes() const;
	void AddMeshes() const;
	void AddPhysicsObjects() const;
	void AddGameObjects() const;
	void AddPlayers() const;
	void AddThreadPool() const;
	void AddWindow() const;
	void AddRenderer() const;
	void AddAudioManager() const;

	std::vector<std::function<void()>> tableAdditions;

	DataBase* database;
	int* playerIDCount;
};


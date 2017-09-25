#pragma once

#include "../nclgl/Vector3.h"

#include <array>
#include <vector>

class RigidBody;

class SpawnSystem
{
public:
	static void Initialise()
	{
		instance = new SpawnSystem();
	}

	static SpawnSystem* GetInstance()
	{
		return instance;
	}

	void RemovePlayerFromGame(RigidBody* playerRigidBody);
	void ReturnPlayer(RigidBody* playerRigidBody);

	void AddSpawnPoint(const Vector3& spawnPoint);
	void AddSpawnPoints(std::vector<Vector3> spawnPoints);
private:
	SpawnSystem() {}

	virtual ~SpawnSystem()
	{
		Destroy();
	}

	static void Destroy()
	{
		delete instance;
	}

	int ChooseRandomSpawnPoint() const;

	static SpawnSystem* instance;
	std::vector<Vector3> spawnPoints;
};


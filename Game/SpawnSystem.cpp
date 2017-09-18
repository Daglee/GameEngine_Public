#include "SpawnSystem.h"

#include "../GameLogicFSM/MessageSystem.h"
#include "../Physics/RigidBody.h"

#include <random>

const float PLAYER_GRAVITY = -0.1f;

SpawnSystem* SpawnSystem::instance = NULL;

void SpawnSystem::RemovePlayerFromGame(RigidBody* playerRigidBody)
{
	MessageSystem::GetInstance()->Transmit(Log::Hash(playerRigidBody->tag + "dead"), false);

	playerRigidBody->UpdatePosition(playerRigidBody->lastPosition + Vector3(0, 100000, 0));
	playerRigidBody->gravity = 0;
}

void SpawnSystem::ReturnPlayer(RigidBody* playerRigidBody)
{
	playerRigidBody->gravity = PLAYER_GRAVITY;

	int spawnPointIndex = ChooseRandomSpawnPoint();
	playerRigidBody->UpdatePosition(spawnPoints[spawnPointIndex]);
}

void SpawnSystem::AddSpawnPoint(const Vector3& newPoint)
{
	spawnPoints.push_back(newPoint);
}

void SpawnSystem::AddSpawnPoints(std::vector<Vector3> spawnPoints)
{
	for each(Vector3 point in spawnPoints)
	{
		spawnPoints.push_back(point);
	}
}

int SpawnSystem::ChooseRandomSpawnPoint()
{
	std::random_device randomGenerator;
	std::mt19937 mersenneTwister(randomGenerator());
	std::uniform_int_distribution<int> range(0, spawnPoints.size() - 1);

	int randomSpawnPoint = range(mersenneTwister);

	return randomSpawnPoint;
}

#include "SpawnSystem.h"

#include "../../GameLogicFSM/MessageSystem.h"
#include "../../Physics/RigidBody.h"

#include <random>

const float PLAYER_GRAVITY = -0.1f;

SpawnSystem* SpawnSystem::instance = nullptr;

void SpawnSystem::RemovePlayerFromGame(RigidBody* playerRigidBody)
{
	MessageSystem::GetInstance()->TransmitMessage(Log::Hash(playerRigidBody->tag + "dead"));

	playerRigidBody->UpdatePosition(playerRigidBody->lastPosition + Vector3(0, 100000, 0));
	playerRigidBody->gravity = 0;
}

void SpawnSystem::ReturnPlayer(RigidBody* playerRigidBody)
{
	playerRigidBody->gravity = PLAYER_GRAVITY;

	const int spawnPointIndex = ChooseRandomSpawnPoint();
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

int SpawnSystem::ChooseRandomSpawnPoint() const
{
	random_device randomGenerator;
	mt19937 mersenneTwister(randomGenerator());
	const uniform_int_distribution<int> range(0, spawnPoints.size() - 1);

	const int randomSpawnPoint = range(mersenneTwister);

	return randomSpawnPoint;
}

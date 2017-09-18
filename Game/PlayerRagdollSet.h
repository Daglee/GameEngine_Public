#pragma once

class Ragdoll;
class PhysicsEngine;
class Renderer;
class DataBase;
class CharacterModel;
class DataBase;
class Matrix4;

#include <vector>

class PlayerRagdollSet
{
public:
	PlayerRagdollSet(int ragdollLimit, CharacterModel* playerModel,
		Renderer* renderer, PhysicsEngine* physicsEngine);
	~PlayerRagdollSet();

	void SpawnNextRagdoll(const DataBase* database, const Matrix4& playerRotation);

	void SetRenderer(Renderer* renderer);
	void SetPhysicsEngine(PhysicsEngine* physicsEngine);

private:
	bool MaxRagdollsSpawned();
	void DespawnRagdolls();

	std::vector<Ragdoll*> ragdolls;

	int ragdollLimit;
	int numberOfSpawnedRagdolls = 0;
};


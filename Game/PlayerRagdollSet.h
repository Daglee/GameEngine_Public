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
	PlayerRagdollSet(const int ragdollLimit, CharacterModel* playerModel,
		Renderer* renderer, PhysicsEngine* physicsEngine);
	~PlayerRagdollSet();

	void SpawnNextRagdoll(DataBase* database, const Matrix4& playerRotation);

	void SetRenderer(Renderer* renderer) const;
	void SetPhysicsEngine(PhysicsEngine* physicsEngine) const;

private:
	bool MaxRagdollsSpawned() const;
	void DespawnRagdolls();

	std::vector<Ragdoll*> ragdolls;

	int ragdollLimit;
	int numberOfSpawnedRagdolls = 0;
};


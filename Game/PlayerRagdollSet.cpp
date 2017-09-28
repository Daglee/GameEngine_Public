#include "PlayerRagdollSet.h"

#include "Ragdoll.h"
#include "../nclgl/Renderer.h"
#include "../Physics/PhysicsEngine.h"
#include "Structure/PhysicsObject.h"
#include "CharacterModel.h"
#include "../ResourceManagment/DataBase.h"
#include "../nclgl/Matrix4.h"

PlayerRagdollSet::PlayerRagdollSet(const int ragdollLimit, CharacterModel* playerModel,
	Renderer* renderer, PhysicsEngine* physicsEngine)
{
	this->ragdollLimit = ragdollLimit;

	for (int i = 0; i < ragdollLimit; ++i)
	{
		ragdolls.push_back(new Ragdoll(playerModel, renderer, physicsEngine));
	}
}

PlayerRagdollSet::~PlayerRagdollSet()
{
	for each (Ragdoll* body in ragdolls)
	{
		delete body;
	}
}

void PlayerRagdollSet::SpawnNextRagdoll(DataBase* database, const Matrix4& playerRotation)
{
	if (MaxRagdollsSpawned())
	{
		DespawnRagdolls();
	}

	ragdolls[numberOfSpawnedRagdolls]->SpawnRagdoll(database, playerRotation);
	++numberOfSpawnedRagdolls;
}

bool PlayerRagdollSet::MaxRagdollsSpawned() const
{
	return numberOfSpawnedRagdolls == ragdollLimit;
}

void PlayerRagdollSet::DespawnRagdolls()
{
	for each (Ragdoll* body in ragdolls)
	{
		body->DeleteRagdoll();
	}

	numberOfSpawnedRagdolls = 0;
}

void PlayerRagdollSet::SetRenderer(Renderer* renderer) const
{
	for each (Ragdoll* body in ragdolls)
	{
		body->SetRenderer(renderer);
	}
}

void PlayerRagdollSet::SetPhysicsEngine(PhysicsEngine* physicsEngine) const
{
	for each (Ragdoll* body in ragdolls)
	{
		body->SetPhysicsEngine(physicsEngine);
	}
}
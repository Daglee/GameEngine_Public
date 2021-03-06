#pragma once

#include <vector>

class PhysicsEngine;
class Renderer;
class DataBase;
class Matrix4;
class PhysicsObject;
class CharacterModel;
class RigidBody;

class Ragdoll
{
public:
	Ragdoll(CharacterModel* playerModel, Renderer* renderer, PhysicsEngine* physicsEngine);

	~Ragdoll()
	{
		DeleteRagdoll();
	}

	void SpawnRagdoll(DataBase* database, const Matrix4& playerRotation);
	void DeleteRagdoll();

	void SetRenderer(Renderer* renderer)
	{
		this->renderer = renderer;
	}

	void SetPhysicsEngine(PhysicsEngine* physicsEngine)
	{
		this->physicsEngine = physicsEngine;
	}

private:
	void RemoveLimbFromSubsystems(PhysicsObject* limb) const;

	void InitialiseRigidBody(RigidBody* rigidBody);

	std::vector<PhysicsObject*> ragdollLimbs;

	Renderer* renderer;
	PhysicsEngine* physicsEngine;
	CharacterModel* playerModel;
};


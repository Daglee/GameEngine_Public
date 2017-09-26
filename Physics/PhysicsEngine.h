#pragma once

#include "../ResourceManagment/Resource.h"
#include "../Game/Subsystem.h"
#include "ExplosionSet.h"
#include "../Threading/ThreadPool.h"
#include "CollisionResponse.h"
#include "Collider.h"

#include <atomic>
#include <mutex>
#include <vector>

class Renderer;
class DataBase;
class RigidBody;

class PhysicsEngine : public Resource, public Subsystem
{
public:
	/*
	  A mutex is needed in the update function to make sure
	  no rigid bodies are added or removed during the update.
	  It stops a rigid body suddenly being removed by another
	  thread in the middle of, for example, broadphase.

	  Performance loss is negligible and it stops a whole
	  load of errors.
	*/
	mutex update_mutex;

	PhysicsEngine(Renderer* renderer, DataBase* database);
	~PhysicsEngine() 
	{
		delete explosions;
	}

	void Update(const float& deltatime) override;

	void AddRigidBody(RigidBody* rigidBody);
	void RemoveRigidBody(RigidBody* rigidBody);

	void Read(string resourcename) override;
	void ReadParams(string params) override;

private:
	Renderer* renderer;
	DataBase* database;

	vector<RigidBody*> rigidBodies;
	vector<RigidBody*> narrowPhaseDeleteBuffer;

	void UpdatePositions(float msec) const;

	void BroadPhase(); //Sort and sweep!
	void BroadPhaseChunk(const int& start, const int& end);
	void SortRigidBodiesAlongAxis(const Vector3& axis);

	void NarrowPhase();

	static bool compareRigidBodies(RigidBody* a, RigidBody* b);

	void Explosion(CollisionPair collisionPair) const;
	ExplosionSet* explosions;

	void AnnounceAndDeleteBasicCollisions(const CollisionPair& collisionPair);
	void ExplodeIfNotIgnore(const CollisionPair& collisionPair);
	void AnnounceCollision(const std::string& aTag, const std::string& bTag);
	void DeleteRigidBodyIfColliderContains(const CollisionPair& collisionPair, const std::string& colliderTag);

	void ClearNarrowPhaseDeleteBuffer();

	ThreadPool* threadPool;
	std::atomic<int> collisionPairCounter;
	vector<CollisionPair> collisionPairs;
	int broadPhaseChunkSize;
	mutex deleteBufferMutex;
	mutex positionUpdateMutex;
};


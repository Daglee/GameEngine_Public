#pragma once

/*
  IMPORTANT:
  Physics engine and the other classes in this
  library will be used for CSC3222 coursework.
  All extensions for the game engine have been marked with
  "// BEGIN EXT" and "// END EXT".
*/

#include "RigidBody.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "../ResourceManagment/Resource.h"
#include "../Game/Subsystem.h"
#include "ExplosionSet.h"

#include <atomic>
#include <mutex>
#include <vector>
#include <algorithm>

class Renderer;
class DataBase;
#include "../Threading/ThreadPool.h"

struct CollisionPair
{
	RigidBody* r1;
	RigidBody* r2;

	CollisionPair()
	{
		r1 = nullptr;
		r2 = nullptr;
	}

	CollisionPair(RigidBody* r1, RigidBody* r2)
	{
		this->r1 = r1;
		this->r2 = r2;
	}
};

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

	void Update(float deltatime);

	void AddRigidBody(RigidBody* rigidBody);
	void RemoveRigidBody(RigidBody* rigidBody);

	void SemiImplicitEuler(RigidBody& rigidBody, Vector3 gravity, float time);

	void Read(string resourcename);
	void ReadParams(string params);

private:
	Renderer* renderer;
	DataBase* database;

	vector<RigidBody*> rigidBodies;
	vector<RigidBody*> narrowPhaseDeleteBuffer;

	void UpdatePositions(float msec);

	void BroadPhase(); //Sort and sweep!
	void BroadPhaseChunk(const int& start, const int& end);
	void SortRigidBodiesAlongAxis(Vector3& axis);
	void NarrowPhase();

	static bool compareRigidBodies(RigidBody* a, RigidBody* b);

	void ImpulseResponse(CollisionPair collisionPair, Vector3 contactNormal, float penetrationDepth);
	void ProjectionResponse(CollisionPair collisionPair, Vector3 contactNormal,
		float penetrationDepth, float aInverseMass, float bInverseMass);

	void Explosion(CollisionPair collisionPair);
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
};


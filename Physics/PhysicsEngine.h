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
#include "../ResourceManagment/ResourceBase.h"
#include "../Game/Subsystem.h"
#include "ExplosionSet.h"

#include <mutex>
#include <vector>
#include <algorithm>

class Renderer;
class DataBase;

struct CollisionPair
{
	RigidBody* r1;
	RigidBody* r2;
};

class PhysicsEngine : public ResourceBase, public Subsystem
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
	//vector<GameObject*> explosions;
	vector<RigidBody*> narrowPhaseDeleteBuffer;

	void UpdatePositions(float msec);

	vector<CollisionPair> BroadPhase(); //Sort and sweep!
	void SortRigidBodiesAlongAxis(Vector3& axis);
	void NarrowPhase(vector<CollisionPair> pairs);

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
};


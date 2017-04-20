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

#include <mutex>
#include <vector>
#include <algorithm>

const float AT_REST = 0.001f;

struct CollisionPair
{
	RigidBody* r1;
	RigidBody* r2;
};

class PhysicsEngine : public ResourceBase, public Subsystem
{
public:
	//	BEGIN EXT
	/*
	  A mutex is needed in the update function to make sure
	  no rigid bodies are added or removed during the update.
	  It stops a rigid body suddenly being removed by another
	  thread in the middle of, for example, broadphase.

	  Performance loss is negligible and it stops a whole
	  load of errors.
	*/
	mutex update_mutex;
	//	END EXT

	PhysicsEngine();
	~PhysicsEngine() {}

	void Update(float deltatime);

	void AddRigidBody(RigidBody* r);
	void RemoveRigidBody(RigidBody* r);

	void semiImplicitEuler(RigidBody& r, Vector3 gravity, float time);
	
	//	BEGIN EXT
	void Read(string resourcename);
	void ReadParams(string params);
	//	END EXT

private:
	vector<RigidBody*> rigidBodies;

	void updatePositions(float sec);

	vector<CollisionPair> BroadPhase(); //Sort and sweep!
	void NarrowPhase(vector<CollisionPair> pairs);

	static bool compareRigidBodies(RigidBody* a, RigidBody* b);

	void ImpulseResponse(CollisionPair collisionPair, Vector3 contactNormal, float penetrationDepth);
	void ProjectionResponse(CollisionPair collisionPair, Vector3 contactNormal, 
		float penetrationDepth, float aInverseMass, float bInverseMass);
};


#include "PhysicsEngine.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../ResourceManagment/Log.h"

PhysicsEngine::PhysicsEngine() : ResourceBase() 
{
	this->SetResourceSize(sizeof(*this));
}

void PhysicsEngine::AddRigidBody(RigidBody* r) 
{
	/*
	  Is the update busy? If not, add. If it is,
	  then just wait.
	*/
	unique_lock<mutex> lock(update_mutex);

	rigidBodies.push_back(r);
}

void PhysicsEngine::RemoveRigidBody(RigidBody* r) 
{
	/*
	  Is the update busy? If not, remove. If it is, 
	  then just wait.
	*/
	unique_lock<mutex> lock(update_mutex);

	rigidBodies.erase(std::remove(rigidBodies.begin(),
		rigidBodies.end(), r), rigidBodies.end());
}

void PhysicsEngine::Update(float sec)
{
	/*
	  LOCK! Nothing in the engine is allowed
	  to change until the update is done.
	*/
	lock_guard<mutex> lock(update_mutex);

	updateTimer.StartTimer();

	updatePositions(sec);

	/*
	  Narrow phase performed on the rigidbody 
	  pairs that broad phase collected
	*/
	NarrowPhase(BroadPhase());

	updateTimer.StopTimer();
}

void PhysicsEngine::updatePositions(float sec) 
{
	for each (RigidBody* r in rigidBodies) {
		if (!(r->isStatic || r->atRest)) {
			semiImplicitEuler(*r, Vector3(0, r->gravity, 0), sec);
		}
	}
}

void PhysicsEngine::semiImplicitEuler(RigidBody& r, Vector3 gravity, float deltaTime) 
{
	r.velocity = (r.velocity + ((r.acceleration + gravity) * deltaTime)) * r.drag;

	//We need to precalculate what the new displacement will be to use it in our rest
	//detection, but we only apply it if the body is not now considered at rest.
	Vector3 disp = r.collider->position + (r.velocity * deltaTime);

	//If the object has barely moved for 10 frames then set it to rest
	if (((disp - r.lastPosition) / deltaTime).sqrLength() < AT_REST)
	{
		++r.restFrames;
		if (r.restFrames > 10)
		{
			//r.atRest = true;
			return;
		}
	}
	else r.restFrames = 0;

	//if the object is not at rest, then update it's position and last displacement
	r.UpdatePosition(disp);
	r.lastPosition = r.collider->position;
}

//Sort and Sweep
vector<CollisionPair> PhysicsEngine::BroadPhase() 
{
	vector<CollisionPair> pairs;

	for each (RigidBody* r in rigidBodies) {
		r->collider->ProjectOnAxis(Vector3(1, 0, 0));
	}

	std::sort(rigidBodies.begin(), rigidBodies.end(), compareRigidBodies);

	for (int x = 0; x < rigidBodies.size(); ++x) {
		for (int y = x + 1; y < rigidBodies.size(); ++y) {

			//Are they possibly colliding?
			if (rigidBodies[x]->collider->objbounds.max > rigidBodies[y]->collider->objbounds.min) {

				//Yes, get Narrow phase to check this pair properly...
				CollisionPair p;
				p.r1 = rigidBodies[x];
				p.r2 = rigidBodies[y];
				pairs.push_back(p);
			}
		}
	}
	return pairs;
}

bool PhysicsEngine::compareRigidBodies(RigidBody* a, RigidBody* b) 
{
	return a->collider->objbounds.min < b->collider->objbounds.min;
}

void PhysicsEngine::NarrowPhase(vector<CollisionPair> pairs)
{
	for each (CollisionPair collisionPair in pairs) {
		Vector3 contactNormal;
		float penetrationDepth;

		if (collisionPair.r1->collider->IsColliding(contactNormal, *(collisionPair.r2->collider), penetrationDepth)) {
			//Add them to the game logic in case anything else needs to happen.
			if (gamelogic != nullptr && 
				collisionPair.r1->tag != "" && //If they dont have a tag then they're probably not needed!
				collisionPair.r2->tag != "") {
				gamelogic->colliders.insert({ collisionPair.r1, collisionPair.r2 });
			}

			ImpulseResponse(collisionPair, contactNormal, penetrationDepth);
		}
	}
}

void PhysicsEngine::ImpulseResponse(CollisionPair collisionPair, Vector3 contactNormal, 
	float penetrationDepth) 
{
	Vector3 vab = collisionPair.r1->velocity - collisionPair.r2->velocity;

	//Reciprocals of masses
	float aMass = collisionPair.r1->isStatic ? 0 : collisionPair.r1->inverseMass;
	float bMass = collisionPair.r2->isStatic ? 0 : collisionPair.r2->inverseMass;

	float e = 0.999f;

	float J = (-(1.0f + e) * Vector3::Dot(vab, contactNormal)) /
		(Vector3::Dot(contactNormal, contactNormal * (aMass + bMass)));

	//Fix the overlap first
	ProjectionResponse(collisionPair, contactNormal, penetrationDepth, aMass, bMass);

	//No send them flying apart (maybe)...
	Vector3 aVelocity = (contactNormal * (J * aMass));
	Vector3 bVelocity = (contactNormal * (J * bMass));
	collisionPair.r1->velocity += aVelocity;
	collisionPair.r2->velocity -= bVelocity;

	//No longer at rest?
	if (aVelocity.sqrLength() > AT_REST) collisionPair.r1->atRest = false;
	if (bVelocity.sqrLength() > AT_REST) collisionPair.r2->atRest = false;
}

void PhysicsEngine::ProjectionResponse(CollisionPair collisionPair, Vector3 contactNormal, 
	float penetrationDepth, float aInverseMass, float bInverseMass) 
{
	float moveratio = aInverseMass / (aInverseMass + bInverseMass);

	Vector3 xPos = collisionPair.r1->collider->position;
	Vector3 xOff = xPos - (contactNormal * penetrationDepth * moveratio);

	Vector3 yPos = collisionPair.r2->collider->position;
	Vector3 yOff = yPos + (contactNormal * penetrationDepth * (1.0 - moveratio));

	collisionPair.r1->UpdatePosition(xOff);
	collisionPair.r2->UpdatePosition(yOff);
}

void PhysicsEngine::Read(string resourcename)
{
	this->SetResourceName(resourcename);
}

void PhysicsEngine::ReadParams(string params) 
{
	Read(params);
}
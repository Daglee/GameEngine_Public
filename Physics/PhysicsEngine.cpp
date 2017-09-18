#include "PhysicsEngine.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../ResourceManagment/Log.h"
#include "../GameLogicFSM/MessageSystem.h"
#include "../nclgl/Renderer.h"
#include "../ResourceManagment/DataBase.h"

#define EXPLOSION_RADIUS 100
#define EXPLOSION_FORCE 1000

PhysicsEngine::PhysicsEngine(Renderer* rend, DataBase* db) : ResourceBase()
{
	renderer = rend;
	database = db;

	this->SetResourceSize(sizeof(*this));
}

void PhysicsEngine::AddRigidBody(RigidBody* r) 
{
	//	BEGIN EXT
	/*
	  Is the update busy? If not, add. If it is,
	  then just wait.
	*/
	unique_lock<mutex> lock(update_mutex);
	//	END EXT

	rigidBodies.push_back(r);
}

void PhysicsEngine::RemoveRigidBody(RigidBody* r) 
{
	//	BEGIN EXT
	/*
	  Is the update busy? If not, remove. If it is, 
	  then just wait.
	*/
	unique_lock<mutex> lock(update_mutex);
	//	END EXT

	rigidBodies.erase(std::remove(rigidBodies.begin(),
		rigidBodies.end(), r), rigidBodies.end());
}

void PhysicsEngine::Update(float sec)
{
	//	BEGIN EXT
	/*
	  LOCK! Nothing in the engine is allowed
	  to change until the update is done.
	*/
	lock_guard<mutex> lock(update_mutex);

	updateTimer.StartTimer();
	//	END EXT

	updatePositions(sec);

	/*
	  Narrow phase performed on the rigidbody 
	  pairs that broad phase collected
	*/
	NarrowPhase(BroadPhase());

	RenderExplosions();

	//	BEGIN EXT
	updateTimer.StopTimer();
	//	END EXT
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

	for (unsigned x = 0; x < rigidBodies.size(); ++x) {
		for (unsigned y = x + 1; y < rigidBodies.size(); ++y) {

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
	vector<RigidBody*> delete_buffer;

	for each (CollisionPair collisionPair in pairs)
	{
		Vector3 contactNormal;
		float penetrationDepth;

		if (collisionPair.r1->collider->IsColliding(contactNormal, *(collisionPair.r2->collider), penetrationDepth)) {
			//	BEGIN EXT
			//Add them to the game logic in case anything else needs to happen.
			if (collisionPair.r1->tag != "" && //If they dont have a tag then they're probably not needed!
				collisionPair.r2->tag != "") {

				if (collisionPair.r1->ignoreTag != "" || collisionPair.r2->ignoreTag != "") {
					if (collisionPair.r1->ignoreTag != collisionPair.r2->tag &&
						collisionPair.r2->ignoreTag != collisionPair.r1->tag) {

						MessageSystem::GetInstance()->BeginEvent(Log::Hash(collisionPair.r1->tag + "_colliding_" + collisionPair.r2->tag));
						MessageSystem::GetInstance()->BeginEvent(Log::Hash(collisionPair.r2->tag + "_colliding_" + collisionPair.r1->tag));

						if (collisionPair.r1->tag.find("bullet") != std::string::npos) {
							delete_buffer.push_back(collisionPair.r1);
						}
						else if (collisionPair.r2->tag.find("bullet") != std::string::npos) {
							delete_buffer.push_back(collisionPair.r2);
						}
					}
				}
				else {
					if (collisionPair.r1->secondarytag.find("pickup") != std::string::npos || collisionPair.r2->secondarytag.find("pickup") != std::string::npos) {
						MessageSystem::GetInstance()->TransmitMessage(Log::Hash(collisionPair.r1->tag + "_colliding_" + collisionPair.r2->tag));
						MessageSystem::GetInstance()->TransmitMessage(Log::Hash(collisionPair.r1->secondarytag));
						MessageSystem::GetInstance()->TransmitMessage(Log::Hash(collisionPair.r2->secondarytag));
					}
					else {
						MessageSystem::GetInstance()->TransmitMessage(Log::Hash(collisionPair.r1->tag + "_colliding_" + collisionPair.r2->tag));
						MessageSystem::GetInstance()->TransmitMessage(Log::Hash(collisionPair.r2->tag + "_colliding_" + collisionPair.r1->tag));
					}
				}

			}//	END EXT

			if (collisionPair.r1->secondarytag == "explosion" || collisionPair.r2->secondarytag == "explosion") {
				if (collisionPair.r1->ignoreTag != collisionPair.r2->tag &&
					collisionPair.r2->ignoreTag != collisionPair.r1->tag) {
					Explosion(collisionPair);

					if (collisionPair.r1->secondarytag == "explosion") delete_buffer.push_back(collisionPair.r1);
					else delete_buffer.push_back(collisionPair.r2);
				}
			}
			else ImpulseResponse(collisionPair, contactNormal, penetrationDepth);
		}
	}

	for each (RigidBody* rb in delete_buffer)
	{
		rigidBodies.erase(std::remove(rigidBodies.begin(),
			rigidBodies.end(), rb), rigidBodies.end());
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
	Vector3 yOff = yPos + (contactNormal * penetrationDepth * (1.0f - moveratio));

	collisionPair.r1->UpdatePosition(xOff);
	collisionPair.r2->UpdatePosition(yOff);
}

//	BEGIN EXT
void PhysicsEngine::Explosion(CollisionPair collisionPair)
{
	RigidBody* explosion;
	RigidBody* entity;

	//Which one is the explosion?
	if (collisionPair.r1->secondarytag == "explosion") {
		explosion = collisionPair.r1;
		entity = collisionPair.r2;
	}
	else {
		explosion = collisionPair.r2;
		entity = collisionPair.r1;
	}

	//Find any rigidbodies within its radius.
	for each (RigidBody* r in rigidBodies)
	{
		if (r != explosion) {
			Vector3 distance = explosion->lastPosition - r->lastPosition;
			distance.x = std::abs(distance.x);
			distance.y = std::abs(distance.y);
			distance.z = std::abs(distance.z);

			if (distance.Length() < Vector3(EXPLOSION_RADIUS, EXPLOSION_RADIUS, EXPLOSION_RADIUS).Length()) {
				ExplosionResponse(explosion, r);
				MessageSystem::GetInstance()->TransmitMessage(Log::Hash(entity->tag + "_colliding_" + explosion->tag));
				MessageSystem::GetInstance()->TransmitMessage(Log::Hash(explosion->tag + "_colliding_" + entity->tag));
			}
		}
	}

	InitialiseExplosion(explosion->lastPosition, EXPLOSION_RADIUS);
}

void PhysicsEngine::ExplosionResponse(RigidBody* explosion, RigidBody* rb)
{
	Vector3 direction = rb->lastPosition - explosion->lastPosition;

	float length = direction.Length();
	Vector3 unit = direction / length;

	//Apply force away from centre of explosion.
	rb->ApplyForce(unit * EXPLOSION_FORCE);
}

void PhysicsEngine::InitialiseExplosion(Vector3 Position, float radius)
{
	GameObject* ex = new GameObject(*renderer);
	ex->AddMesh(*database->OBJMeshes->Find("../Data/Meshes/sphere.obj"));
	ex->SetPosition(Position);
	ex->SetSize(Vector3(30, 30, 30));
	ex->GetSceneNode()->SetColour(Vector4(1, 0, 0, 1));

	explosions.push_back(ex);
}

void PhysicsEngine::RenderExplosions()
{
	//Set a starting size and colour.
	for each (GameObject* explosion in explosions)
	{
		explosion->SetSize(explosion->GetSize() + Vector3(0.3f, 0.3f, 0.3f));
		explosion->GetSceneNode()->SetColour(explosion->GetSceneNode()->GetColour() - Vector4(0, -0.002f, 0, 1.0f));
	}

	//If its reached its size limit, delete it.
	vector <GameObject*> delete_buffer;
	for each (GameObject* explosion in explosions)
	{
		if (explosion->GetSize().Length() >= Vector3(EXPLOSION_RADIUS, EXPLOSION_RADIUS, EXPLOSION_RADIUS).Length()) {
			delete_buffer.push_back(explosion);
		}
	}

	for each (GameObject* explosion in delete_buffer)
	{
		explosions.erase(std::remove(explosions.begin(),
			explosions.end(), explosion), explosions.end());
		renderer->RemoveSceneNode(explosion->GetSceneNode());
	}
}


void PhysicsEngine::Read(string resourcename)
{
	this->SetResourceName(resourcename);
}

void PhysicsEngine::ReadParams(string params) 
{
	Read(params);
}
//	END EXT
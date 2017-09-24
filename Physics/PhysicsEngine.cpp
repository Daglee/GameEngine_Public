#include "PhysicsEngine.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../ResourceManagment/Log.h"
#include "../GameLogicFSM/MessageSystem.h"
#include "../nclgl/Renderer.h"
#include "../ResourceManagment/DataBase.h"
#include "../Threading/TaskFuture.h"

#include <functional>

const int FRAMES_UNTIL_AT_REST = 10;
const float MAX_AT_REST_VELOCITY = 0.001f;
const int BROAD_PHASE_THREADS = 3;

PhysicsEngine::PhysicsEngine(Renderer* renderer, DataBase* database) : Resource()
{
	this->renderer = renderer;
	this->database = database;

	explosions = new ExplosionSet(renderer, database);

	threadPool = static_cast<ThreadPool*>(database->GetTable("GThreadPool")->GetResources()->Find("ThreadPool"));

	this->SetSizeInBytes(sizeof(*this));
}

void PhysicsEngine::AddRigidBody(RigidBody* rigidBody)
{
	/*
	  Is the update busy? If not, add. If it is,
	  then just wait.
	*/
	unique_lock<mutex> lock(update_mutex);

	rigidBodies.push_back(rigidBody);

	collisionPairs = vector<CollisionPair>(rigidBodies.size() * rigidBodies.size(), CollisionPair());

	broadPhaseChunkSize = rigidBodies.size() / BROAD_PHASE_THREADS;
}

void PhysicsEngine::RemoveRigidBody(RigidBody* rigidBody)
{
	/*
	  Is the update busy? If not, remove. If it is,
	  then just wait.
	*/
	unique_lock<mutex> lock(update_mutex);

	rigidBodies.erase(std::remove(rigidBodies.begin(),
		rigidBodies.end(), rigidBody), rigidBodies.end());

	collisionPairs = vector<CollisionPair>(rigidBodies.size() * rigidBodies.size(), CollisionPair());

	broadPhaseChunkSize = rigidBodies.size() / BROAD_PHASE_THREADS;
}

void PhysicsEngine::Update(float sec)
{
	/*
	  LOCK! Nothing in the engine is allowed
	  to change until the update is done.
	*/
	lock_guard<mutex> lock(update_mutex);

	updateTimer.StartTimer();

	UpdatePositions(sec);
	BroadPhase();
	NarrowPhase();
	explosions->RenderAllExplosions();

	updateTimer.StopTimer();
}

void PhysicsEngine::UpdatePositions(float msec)
{
	for each (RigidBody* rigidBody in rigidBodies)
	{
		if (!(rigidBody->isStatic || rigidBody->atRest))
		{
			SemiImplicitEuler(*rigidBody, Vector3(0, rigidBody->gravity, 0), msec);
		}
	}
}

void PhysicsEngine::SemiImplicitEuler(RigidBody& rigidBody, Vector3 gravity, float deltaTime)
{
	rigidBody.velocity = (rigidBody.velocity + ((rigidBody.acceleration + gravity) * deltaTime)) * rigidBody.drag;

	//We need to precalculate what the new displacement will be to use it in our rest
	//detection, but we only apply it if the body is not now considered at rest.
	Vector3 displacement = rigidBody.collider->position + (rigidBody.velocity * deltaTime);

	//If the object has barely moved for 10 frames then set it to rest
	if (((displacement - rigidBody.lastPosition) / deltaTime).sqrLength() < MAX_AT_REST_VELOCITY)
	{
		++rigidBody.restFrames;

		if (rigidBody.restFrames > FRAMES_UNTIL_AT_REST)
		{
			return;
		}
	}
	else
	{
		rigidBody.restFrames = 0;
	}

	//if the object is not at rest, then update it's position and last displacement
	rigidBody.UpdatePosition(displacement);
	rigidBody.lastPosition = rigidBody.collider->position;
}

//Sort and Sweep
void PhysicsEngine::BroadPhase()
{
	SortRigidBodiesAlongAxis(Vector3(1, 0, 0));

	vector<TaskFuture<void>> threads;
	collisionPairCounter = 0;

	int startIndex = 0;
	int endIndex = broadPhaseChunkSize;

	for (int i = 0; i < BROAD_PHASE_THREADS - 1; ++i)
	{
		threads.push_back(threadPool->SubmitJob([](PhysicsEngine& physicsEngine, int start, int end)
		{
			physicsEngine.BroadPhaseChunk(start, end);
		}, std::ref(*this), startIndex, endIndex));

		startIndex = endIndex;
		endIndex = startIndex + (broadPhaseChunkSize) - 1;
	}

	threads.push_back(threadPool->SubmitJob([](PhysicsEngine& physicsEngine, int start, int end)
	{
		physicsEngine.BroadPhaseChunk(start, end);
	}, std::ref(*this), startIndex, rigidBodies.size()));
	
	for (auto& task : threads)
	{
		task.Complete();
	}
}

void PhysicsEngine::BroadPhaseChunk(const int& start, const int& end)
{
	for (unsigned x = start; x < end; ++x)
	{
		for (unsigned y = x + 1; y < rigidBodies.size(); ++y)
		{
			//Are they possibly colliding?
			if (rigidBodies[x]->collider->objbounds.max > rigidBodies[y]->collider->objbounds.min)
			{
				//Yes, get Narrow phase to check this pair properly...
				collisionPairs[collisionPairCounter.load()] = CollisionPair(rigidBodies[x], rigidBodies[y]);
				collisionPairCounter++;
			}
		}
	}
}

void PhysicsEngine::SortRigidBodiesAlongAxis(Vector3& axis)
{
	for each (RigidBody* rigidBody in rigidBodies)
	{
		rigidBody->collider->ProjectOnAxis(axis);
	}

	std::sort(rigidBodies.begin(), rigidBodies.end(), compareRigidBodies);
}

bool PhysicsEngine::compareRigidBodies(RigidBody* a, RigidBody* b)
{
	return a->collider->objbounds.min < b->collider->objbounds.min;
}

void PhysicsEngine::NarrowPhase()
{
	//for each (CollisionPair collisionPair in pairs)

	for (int i = 0; i < collisionPairCounter.load(); ++i)
	{
		CollisionPair collisionPair = collisionPairs[i];

		Vector3 contactNormal;
		float penetrationDepth;

		if (collisionPair.r1->collider->IsColliding(contactNormal, *(collisionPair.r2->collider), penetrationDepth))
		{
			AnnounceAndDeleteBasicCollisions(collisionPair);

			if (collisionPair.r1->secondarytag == "explosion" || collisionPair.r2->secondarytag == "explosion")
			{
				ExplodeIfNotIgnore(collisionPair);
			}
			else
			{
				ImpulseResponse(collisionPair, contactNormal, penetrationDepth);
			}
		}
	}

	ClearNarrowPhaseDeleteBuffer();
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
	if (aVelocity.sqrLength() > MAX_AT_REST_VELOCITY)
	{
		collisionPair.r1->atRest = false;
	}

	if (bVelocity.sqrLength() > MAX_AT_REST_VELOCITY)
	{
		collisionPair.r2->atRest = false;
	}
}

void PhysicsEngine::ProjectionResponse(CollisionPair collisionPair, Vector3 contactNormal,
	float penetrationDepth, float aInverseMass, float bInverseMass)
{
	float moveRatio = aInverseMass / (aInverseMass + bInverseMass);

	Vector3 xPosition = collisionPair.r1->collider->position;
	Vector3 xOffset = xPosition - (contactNormal * penetrationDepth * moveRatio);

	Vector3 yPosition = collisionPair.r2->collider->position;
	Vector3 yOffset = yPosition + (contactNormal * penetrationDepth * (1.0f - moveRatio));

	collisionPair.r1->UpdatePosition(xOffset);
	collisionPair.r2->UpdatePosition(yOffset);
}

//	BEGIN EXT
void PhysicsEngine::Explosion(CollisionPair collisionPair)
{
	RigidBody* explosion;
	RigidBody* entity;

	//Which one is the explosion?
	if (collisionPair.r1->secondarytag == "explosion")
	{
		explosion = collisionPair.r1;
		entity = collisionPair.r2;
	}
	else
	{
		explosion = collisionPair.r2;
		entity = collisionPair.r1;
	}

	//Find any rigidbodies within its radius.
	for each (RigidBody* rigidBody in rigidBodies)
	{
		if (rigidBody != explosion)
		{
			if (explosions->InRadiusOfExplosion(explosion, rigidBody))
			{
				explosions->ExplosionResponse(explosion, rigidBody);
			}
		}
	}

	MessageSystem::GetInstance()->TransmitMessage(Log::Hash(entity->tag + "_colliding_" + explosion->tag));
	MessageSystem::GetInstance()->TransmitMessage(Log::Hash(explosion->tag + "_colliding_" + entity->tag));

	explosions->InitialiseExplosion(explosion->lastPosition);
}

void PhysicsEngine::AnnounceAndDeleteBasicCollisions(const CollisionPair& collisionPair)
{
	if (collisionPair.r1->tag != "" && collisionPair.r2->tag != "")
	{
		if (collisionPair.r1->ignoreTag != "" || collisionPair.r2->ignoreTag != "")
		{
			if (!collisionPair.r1->Ignores(collisionPair.r2->tag) &&
				!collisionPair.r2->Ignores(collisionPair.r1->tag))
			{
				AnnounceCollision(collisionPair.r1->tag, collisionPair.r2->tag);
				DeleteRigidBodyIfColliderContains(collisionPair, "bullet");
			}
		}
		else
		{
			AnnounceCollision(collisionPair.r1->tag, collisionPair.r2->tag);
		}
	}
}

void PhysicsEngine::ExplodeIfNotIgnore(const CollisionPair& collisionPair)
{
	if (!collisionPair.r1->Ignores(collisionPair.r2->tag) && !collisionPair.r2->Ignores(collisionPair.r1->tag))
	{
		Explosion(collisionPair);

		if (collisionPair.r1->secondarytag == "explosion")
		{
			narrowPhaseDeleteBuffer.push_back(collisionPair.r1);
		}
		else
		{
			narrowPhaseDeleteBuffer.push_back(collisionPair.r2);
		}
	}
}

void PhysicsEngine::AnnounceCollision(const std::string& aTag, const std::string& bTag)
{
	MessageSystem::GetInstance()->BeginEvent(Log::Hash(aTag + "_colliding_" + bTag));
	MessageSystem::GetInstance()->BeginEvent(Log::Hash(bTag + "_colliding_" + aTag));
}

void PhysicsEngine::DeleteRigidBodyIfColliderContains(const CollisionPair& collisionPair, const std::string& colliderTag)
{
	if (collisionPair.r1->tag.find(colliderTag) != std::string::npos)
	{
		narrowPhaseDeleteBuffer.push_back(collisionPair.r1);
	}
	else if (collisionPair.r2->tag.find(colliderTag) != std::string::npos)
	{
		narrowPhaseDeleteBuffer.push_back(collisionPair.r2);
	}
}

void PhysicsEngine::ClearNarrowPhaseDeleteBuffer()
{
	for each (RigidBody* rigidBody in narrowPhaseDeleteBuffer)
	{
		rigidBodies.erase(std::remove(rigidBodies.begin(),
			rigidBodies.end(), rigidBody), rigidBodies.end());
	}

	narrowPhaseDeleteBuffer.clear();
}

void PhysicsEngine::Read(string resourcename)
{
	this->SetName(resourcename);
}

void PhysicsEngine::ReadParams(string params)
{
	Read(params);
}
//	END EXT
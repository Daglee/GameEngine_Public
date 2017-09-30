#include "PhysicsEngine.h"

#include "../GameLogicFSM/FSMManager.h"
#include "../ResourceManagement/Utilities/ErrorLog.h"
#include "../ResourceManagement/Utilities/StringUtility.h"
#include "../GameLogicFSM/Messaging/MessageSystem.h"
#include "../nclgl/Rendering/Renderer.h"
#include "../ResourceManagement/Database/DataBase.h"
#include "Utilities/SemiImplicitEuler.h"
#include "../Entities/RigidBody.h"

#include <functional>

const int FRAMES_UNTIL_AT_REST = 10;
const float MAX_AT_REST_VELOCITY = 0.001f;
const int BROAD_PHASE_THREADS = 3;
const int NARROW_PHASE_THREADS = 1;
const Vector3 X_AXIS = Vector3(1, 0, 0);

PhysicsEngine::PhysicsEngine(Renderer* renderer, DataBase* database) : Resource()
{
	this->renderer = renderer;
	this->database = database;

	explosions = new ExplosionSet(renderer, database);

	threadPool = static_cast<ThreadPool*>(database->GetTable("GThreadPool")->GetResources()->Find("ThreadPool"));

	SetSizeInBytes(sizeof(*this));
}

void PhysicsEngine::AddRigidBody(RigidBody* rigidBody)
{
	unique_lock<mutex> lock(updateMutex);

	rigidBodies.push_back(rigidBody);

	collisionPairs = vector<CollisionPair>(rigidBodies.size() * rigidBodies.size(), CollisionPair());

	broadPhaseChunkSize = rigidBodies.size() / BROAD_PHASE_THREADS;
}

void PhysicsEngine::RemoveRigidBody(RigidBody* rigidBody)
{
	unique_lock<mutex> lock(updateMutex);

	rigidBodies.erase(remove(rigidBodies.begin(),
	                         rigidBodies.end(), rigidBody), rigidBodies.end());

	collisionPairs = vector<CollisionPair>(rigidBodies.size() * rigidBodies.size(), CollisionPair());

	broadPhaseChunkSize = rigidBodies.size() / BROAD_PHASE_THREADS;
}

void PhysicsEngine::Update(const float& deltatime)
{
	lock_guard<mutex> lock(updateMutex);

	updateTimer.StartTimer();

	UpdatePositions(deltatime);
	BroadPhase();
	NarrowPhase();
	explosions->RenderAllExplosions();

	updateTimer.StopTimer();
}

void PhysicsEngine::UpdatePositions(float msec) const
{
	for each (RigidBody* rigidBody in rigidBodies)
	{
		if (!(rigidBody->isStatic || rigidBody->atRest))
		{
			SemiImplicitEuler::UpdateRigidBody(*rigidBody, Vector3(0, rigidBody->gravity, 0), msec);
		}
	}
}

void PhysicsEngine::BroadPhase()
{
	SortRigidBodiesAlongAxis(X_AXIS);
	collisionPairCounter = 0;

	for (unsigned x = 0; x < rigidBodies.size(); ++x)
	{
		for (unsigned y = x + 1; y < rigidBodies.size(); ++y)
		{
			//Are they possibly colliding?
			if (rigidBodies[x]->collider->objbounds.max > rigidBodies[y]->collider->objbounds.min)
			{
				//Yes, get Narrow phase to check this pair properly...
				collisionPairs[collisionPairCounter] = CollisionPair(rigidBodies[x], rigidBodies[y]);
				++collisionPairCounter;
			}
		}
	}
}

void PhysicsEngine::SortRigidBodiesAlongAxis(const Vector3& axis)
{
	for each (RigidBody* rigidBody in rigidBodies)
	{
		rigidBody->collider->ProjectOnAxis(axis);
	}

	sort(rigidBodies.begin(), rigidBodies.end(), compareRigidBodies);
}

bool PhysicsEngine::compareRigidBodies(RigidBody* a, RigidBody* b)
{
	return a->collider->objbounds.min < b->collider->objbounds.min;
}

void PhysicsEngine::NarrowPhase()
{
	for (int i = 0; i < collisionPairCounter; ++i)
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
				CollisionResponse::ImpulseResponse(collisionPair, contactNormal, penetrationDepth);
			}
		}
	}

	ClearNarrowPhaseDeleteBuffer();
}

void PhysicsEngine::Explosion(CollisionPair collisionPair) const
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

	MessageSystem::GetInstance()->TransmitMessage(StringUtility::Hash(entity->tag + "_colliding_" + explosion->tag));
	MessageSystem::GetInstance()->TransmitMessage(StringUtility::Hash(explosion->tag + "_colliding_" + entity->tag));

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

void PhysicsEngine::AnnounceCollision(const string& aTag, const string& bTag)
{
	MessageSystem::GetInstance()->BeginEvent(StringUtility::Hash(aTag + "_colliding_" + bTag));
	MessageSystem::GetInstance()->BeginEvent(StringUtility::Hash(bTag + "_colliding_" + aTag));
}

void PhysicsEngine::DeleteRigidBodyIfColliderContains(const CollisionPair& collisionPair, const string& colliderTag)
{
	unique_lock<mutex> lock(deleteBufferMutex);

	if (collisionPair.r1->tag.find(colliderTag) != string::npos)
	{
		narrowPhaseDeleteBuffer.push_back(collisionPair.r1);
	}
	else if (collisionPair.r2->tag.find(colliderTag) != string::npos)
	{
		narrowPhaseDeleteBuffer.push_back(collisionPair.r2);
	}
}

void PhysicsEngine::ClearNarrowPhaseDeleteBuffer()
{
	for each (RigidBody* rigidBody in narrowPhaseDeleteBuffer)
	{
		rigidBodies.erase(remove(rigidBodies.begin(),
		                         rigidBodies.end(), rigidBody), rigidBodies.end());
	}

	narrowPhaseDeleteBuffer.clear();
}

void PhysicsEngine::Read(const string resourcename)
{
	this->SetName(resourcename);
}

void PhysicsEngine::ReadParams(const string params)
{
	Read(params);
}

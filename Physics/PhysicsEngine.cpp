#include "PhysicsEngine.h"

#include "../GameLogicFSM/FSMManager.h"
#include "../ResourceManagment/Log.h"
#include "../GameLogicFSM/MessageSystem.h"
#include "../nclgl/Renderer.h"
#include "../ResourceManagment/DataBase.h"
#include "../Threading/TaskFuture.h"
#include "SemiImplicitEuler.h"
#include "RigidBody.h"

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

	rigidBodies.erase(remove(rigidBodies.begin(),
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

	vector<TaskFuture<void>> threads;
	collisionPairCounter = 0;

	int startIndex = 0;
	int endIndex = broadPhaseChunkSize;

	for (int i = 0; i < BROAD_PHASE_THREADS - 1; ++i)
	{
		threads.push_back(threadPool->SubmitJob([](PhysicsEngine& physicsEngine, int start, int end)
	                                        {
		                                        physicsEngine.BroadPhaseChunk(start, end);
	                                        }, ref(*this), startIndex, endIndex));

		startIndex = endIndex;
		endIndex = startIndex + (broadPhaseChunkSize) - 1;
	}

	threads.push_back(threadPool->SubmitJob([](PhysicsEngine& physicsEngine, int start, int end)
                                        {
	                                        physicsEngine.BroadPhaseChunk(start, end);
                                        }, ref(*this), startIndex, rigidBodies.size()));

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
				CollisionResponse::ImpulseResponse(collisionPair, contactNormal, penetrationDepth);
			}
		}
	}

	ClearNarrowPhaseDeleteBuffer();
}

//	BEGIN EXT
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

void PhysicsEngine::AnnounceCollision(const string& aTag, const string& bTag)
{
	MessageSystem::GetInstance()->BeginEvent(Log::Hash(aTag + "_colliding_" + bTag));
	MessageSystem::GetInstance()->BeginEvent(Log::Hash(bTag + "_colliding_" + aTag));
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

void PhysicsEngine::Read(string resourcename)
{
	this->SetName(resourcename);
}

void PhysicsEngine::ReadParams(string params)
{
	Read(params);
}

//	END EXT

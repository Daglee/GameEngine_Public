#include "ExplosionSet.h"

#include "RigidBody.h"
#include "../GameLogicFSM/Messaging/MessageSystem.h"
#include "../Game/Structure/GameObject.h"
#include "../ResourceManagment/DataBase.h"
#include "../nclgl/Renderer.h"

const float EXPLOSION_RADIUS = 100.0f;
const float EXPLOSION_FORCE = 1000.0f;

ExplosionSet::ExplosionSet(Renderer* renderer, DataBase* database)
{
	this->renderer = renderer;
	this->database = database;
}

ExplosionSet::~ExplosionSet()
{
	for each (GameObject* explosion in explosions)
	{
		delete explosion;
	}
}

bool ExplosionSet::InRadiusOfExplosion(const RigidBody* explosion, const RigidBody* rigidBody) const
{
	Vector3 distance = explosion->lastPosition - rigidBody->lastPosition;
	distance.x = abs(distance.x);
	distance.y = abs(distance.y);
	distance.z = abs(distance.z);

	return distance.Length() < Vector3(EXPLOSION_RADIUS, EXPLOSION_RADIUS, EXPLOSION_RADIUS).Length();
}

void ExplosionSet::ExplosionResponse(RigidBody* explosion, RigidBody* rigidBody)
{
	Vector3 direction = rigidBody->lastPosition - explosion->lastPosition;

	const float length = direction.Length();
	const Vector3 unit = direction / length;

	//Apply force away from centre of explosion.
	rigidBody->ApplyForce(unit * EXPLOSION_FORCE);
}

void ExplosionSet::RenderAllExplosions()
{
	ExpandExplosions();
	PrepareDeletionOfMaxSizeExplosions();
	ClearDeleteBuffer();
}

void ExplosionSet::InitialiseExplosion(const Vector3& Position)
{
	GameObject* ex = new GameObject(*renderer);
	ex->AddMesh(
		*static_cast<OBJMesh*>(database->GetTable("OBJMeshes")->GetResources()->Find("../Data/Meshes/sphere.obj")));
	ex->SetPosition(Position);
	ex->SetSize(Vector3(30, 30, 30));
	ex->GetSceneNode()->SetColour(Vector4(1, 0, 0, 1));

	explosions.push_back(ex);
}

void ExplosionSet::ExpandExplosions() const
{
	//Set a starting size and colour.
	for each (GameObject* explosion in explosions)
	{
		explosion->SetSize(explosion->GetSize() + Vector3(0.3f, 0.3f, 0.3f));
		explosion->GetSceneNode()->SetColour(explosion->GetSceneNode()->GetColour() - Vector4(0, -0.002f, 0, 1.0f));
	}
}

void ExplosionSet::PrepareDeletionOfMaxSizeExplosions()
{
	//If its reached its size limit, delete it.
	for each (GameObject* explosion in explosions)
	{
		if (explosion->GetSize().Length() >= Vector3(EXPLOSION_RADIUS, EXPLOSION_RADIUS, EXPLOSION_RADIUS).Length())
		{
			deleteBuffer.push_back(explosion);
		}
	}
}

void ExplosionSet::ClearDeleteBuffer()
{
	for each (GameObject* explosion in deleteBuffer)
	{
		explosions.erase(remove(explosions.begin(),
		                        explosions.end(), explosion), explosions.end());
		renderer->RemoveSceneNode(explosion->GetSceneNode());
	}

	deleteBuffer.clear();
}

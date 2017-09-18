#include "Ragdoll.h"

#include "../nclgl/Renderer.h"
#include "../Physics/PhysicsEngine.h"
#include "PhysicsObject.h"
#include "CharacterModel.h"
#include "../ResourceManagment/DataBase.h"
#include "../nclgl/Matrix4.h"

Ragdoll::Ragdoll(CharacterModel* playerModel, Renderer* renderer, PhysicsEngine* physicsEngine)
{
	this->renderer = renderer;
	this->physicsEngine = physicsEngine;
	this->playerModel = playerModel;
}

void Ragdoll::SpawnRagdoll(const DataBase* database, const Matrix4& playerRotation)
{
	for each (SceneNode* modelLimb in playerModel->modelParts)
	{
		PhysicsObject* limbCopy = new PhysicsObject(renderer, physicsEngine, false, true);

		limbCopy->GetRigidBody()->collider = new SphereCollider(1);
		limbCopy->AddMesh(*database->OBJMeshes->Find("../Data/meshes/robotcube.obj"));

		limbCopy->GetRigidBody()->parentMesh = limbCopy->GetSceneNode();
		limbCopy->SetPosition(modelLimb->GetWorldTransform().GetPositionVector());
		limbCopy->SetSize(modelLimb->GetModelScale());
		limbCopy->GetSceneNode()->SetColour(modelLimb->GetColour());

		InitialiseRigidBody(limbCopy->GetRigidBody());
		limbCopy->GetSceneNode()->SetTransform(playerModel->GetTransform());

		ragdollLimbs.push_back(limbCopy);
	}
}

void Ragdoll::DeleteRagdoll()
{
	for each (PhysicsObject* limb in ragdollLimbs)
	{
		RemoveLimbFromSubsystems(limb);

		delete limb;
	}

	ragdollLimbs.clear();
}

void Ragdoll::RemoveLimbFromSubsystems(PhysicsObject* limb) const
{
	renderer->RemoveSceneNode(limb->GetSceneNode());
	physicsEngine->RemoveRigidBody(limb->GetRigidBody());
}

void Ragdoll::InitialiseRigidBody(RigidBody* rigidBody)
{
	rigidBody->gravity = -0.01f;
	rigidBody->UpdateMass(100.0f);
	rigidBody->drag = 0.9f;
	rigidBody->atRest = false;
}

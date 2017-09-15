#include "PhysicsObject.h"


PhysicsObject::PhysicsObject(Renderer* r, PhysicsEngine* p,
	bool isStatic, bool isSphere) : GameObject(*r)
{
	moveable	= isStatic;

	rigidBody.isStatic		= isStatic;
	this->isSphere			= isSphere;
	rigidBody.parentMesh	= &node;

	p->AddRigidBody(&rigidBody);
	this->SetResourceSize(sizeof(*this));
}

PhysicsObject::PhysicsObject(Renderer* r, PhysicsEngine* p,
	bool isStatic, bool isSphere, RigidBody* rb) : GameObject(*r)
{
	moveable	= isStatic;
	rigidBody	= *rb;

	rigidBody.isStatic	= isStatic;
	this->isSphere		= isSphere;

	p->AddRigidBody(&rigidBody);
	this->SetResourceSize(sizeof(*this));
}

PhysicsObject::PhysicsObject() : GameObject()
{
	moveable	= false;

	rigidBody.isStatic		= moveable;
	this->isSphere			= true;
	rigidBody.parentMesh	= &node;

	this->SetResourceSize(sizeof(*this));
}

void PhysicsObject::SetEntity(RigidBody* rb, Renderer* renderer, PhysicsEngine* physics)
{
	if (!rigidBody.initialised) {
		rigidBody = *rb;

		renderer->AddSceneNode(rigidBody.parentMesh);
		physics->AddRigidBody(&rigidBody);
	}
	else {
		renderer->RemoveSceneNode(&node);
		physics->RemoveRigidBody(&rigidBody);

		node = *rb->parentMesh;
		rigidBody = *rb;

		renderer->AddSceneNode(rigidBody.parentMesh);
		physics->AddRigidBody(&rigidBody);
	}
}

void PhysicsObject::SetSize(const Vector3& size)
{
	node.SetModelScale(size);
	scale = size;

	if (isSphere) {
		SphereCollider* rb = new SphereCollider(scale.x);
		rigidBody.collider = rb;
	}

	rigidBody.collider->parentsize = size;
	rigidBody.collider->position = position;
}

void PhysicsObject::Read(string resourcename)
{
	this->SetResourceName(resourcename);
}

void PhysicsObject::ReadParams(string params)
{
	Read(params);
}


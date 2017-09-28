#include "RigidBody.h"
#include "../Colliders/Collider.h"

RigidBody::RigidBody(const Vector3 acceleration, const Vector3 velocity,
                     const float mass, const float drag, const float gravity, const string tag)
{
	this->acceleration = acceleration;
	this->velocity = velocity;

	this->mass = mass;
	inverseMass = 1.0f / mass;

	this->drag = drag;
	this->gravity = gravity;

	parentMesh = nullptr;
	collider = nullptr;
	isStatic = false;
	atRest = false;
	restFrames = 0;
	lastPosition = Vector3();

	this->tag = tag;

	initialised = true;
}

RigidBody::~RigidBody()
{
	delete collider;
}

void RigidBody::ApplyForce(const Vector3& force)
{
	velocity += (force * inverseMass);
}

void RigidBody::UpdatePosition(const Vector3& pos)
{
	lastPosition = pos;

	if (parentMesh)
	{
		Matrix4 transform = parentMesh->GetTransform();
		transform.SetPositionVector(pos);
		parentMesh->SetTransform(transform);
	}

	collider->position = pos;
}

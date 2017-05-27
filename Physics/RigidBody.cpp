#include "RigidBody.h"
#include "Collider.h"

RigidBody::RigidBody(Vector3 acceleration, Vector3 velocity, 
	float mass, float drag, float gravity, std::string tag)
{
	this->acceleration = acceleration;
	this->velocity	= velocity;
	
	this->mass		= mass;
	inverseMass		= 1.0f / mass;

	this->drag		= drag;
	this->gravity	= gravity;

	collider	 = NULL;
	isStatic	 = false;
	atRest		 = false;
	restFrames	 = 0;
	lastPosition = Vector3();

	this->tag = tag;

	initialised = true;
}

RigidBody::~RigidBody()
{
	delete collider;
}

void RigidBody::ApplyForce(Vector3 force)
{
	velocity += (force * inverseMass);
}

void RigidBody::UpdatePosition(Vector3 pos)
{
	lastPosition = pos;

	if (parentMesh) {
		Matrix4 transform = parentMesh->GetTransform();
		transform.SetPositionVector(pos);
		parentMesh->SetTransform(transform);
	}

	collider->position = pos;
}
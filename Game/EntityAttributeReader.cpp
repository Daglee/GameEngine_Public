#include "EntityAttributeReader.h"

#include "Structure/PhysicsObject.h"
#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"

EntityAttributeReader::EntityAttributeReader(const vector<string> tokens, GameObject * entity)
{
	this->tokens = tokens;
	this->entity = entity;
}

void EntityAttributeReader::SetSize()
{
	const float x = stof(tokens.at(1));
	const float y = stof(tokens.at(2));
	const float z = stof(tokens.at(3));

	const Vector3 size(x, y, z);

	entity->SetSize(size);
}

void EntityAttributeReader::SetColour()
{
	const float r = stof(tokens.at(1));
	const float g = stof(tokens.at(2));
	const float b = stof(tokens.at(3));
	const float a = stof(tokens.at(4));

	const Vector4 col(r, g, b, a);

	entity->GetSceneNode()->SetColour(col);
}

void EntityAttributeReader::SetTexture()
{
	entity->SetTexture(*(tokens.at(1)).c_str());
}

void EntityAttributeReader::SetGravity()
{
	PhysicsObject* physicsEntity = static_cast<PhysicsObject*>(entity);

	const float gravity = stof(tokens.at(1));
	physicsEntity->GetRigidBody()->gravity = gravity;
}

void EntityAttributeReader::SetMass()
{
	PhysicsObject* physicsEntity = static_cast<PhysicsObject*>(entity);

	const float mass = stof(tokens.at(1));
	physicsEntity->GetRigidBody()->UpdateMass(mass);
}

void EntityAttributeReader::SetDamping()
{
	PhysicsObject* physicsEntity = static_cast<PhysicsObject*>(entity);

	const float damping = stof(tokens.at(1));
	physicsEntity->GetRigidBody()->drag = damping;
}

void EntityAttributeReader::SetMoveable()
{
	PhysicsObject* physicsEntity = static_cast<PhysicsObject*>(entity);

	const bool isMoveable = atoi(tokens.at(1).c_str()) != 0;
	physicsEntity->GetRigidBody()->isStatic = isMoveable;
}

void EntityAttributeReader::SetAtRest()
{
	PhysicsObject* physicsEntity = static_cast<PhysicsObject*>(entity);

	const bool atRest = atoi(tokens.at(1).c_str()) != 0;
	physicsEntity->GetRigidBody()->atRest = atRest;
}

void EntityAttributeReader::SetTag()
{
	PhysicsObject* physicsEntity = static_cast<PhysicsObject*>(entity);

	const string tag = tokens.at(1);
	physicsEntity->GetRigidBody()->tag = tag;
}

void EntityAttributeReader::SetRotation()
{
	const Vector3 axis(stof(tokens.at(2)), stof(tokens.at(3)), stof(tokens.at(4)));
	const float degrees = stof(tokens.at(1));

	entity->GetSceneNode()->SetTransform(entity->GetSceneNode()->GetTransform() *
		Matrix4::Rotation(degrees, axis));
}
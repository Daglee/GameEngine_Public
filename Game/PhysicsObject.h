#pragma once

#include "GameObject.h"
#include "../Physics/RigidBody.h"

class RigidBody;
class PhysicsEngine;
class Renderer;
class Vector3;

/*
  Wrapper for physics entity.
   - links a scene node to the entity.
   Therefore ny changes applied to the physics entity
   apply to to the scenenode.
*/
class PhysicsObject : public GameObject
{
public:
	PhysicsObject(Renderer* renderer, PhysicsEngine* physicsEngine,
		const bool isStatic, const bool isSphere);

	PhysicsObject(Renderer* renderer, PhysicsEngine* physicsEngine,
		const bool isStatic, const bool isSphere,
		RigidBody* rb);

	PhysicsObject();

	~PhysicsObject() {}

	void SetEntity(RigidBody* rb, Renderer* renderer, PhysicsEngine* physics);
	void AddToPhysicsEngine(PhysicsEngine* p);

	inline void ApplyForce(const Vector3& newtons)
	{
		rigidBody.ApplyForce(newtons);
	}

	Vector3 GetPosition() const override;
	void SetPosition(const Vector3& pos) override;

	Vector3 GetSize() const override;
	void SetSize(const Vector3& size) override;

	bool IsMoveable() const;
	void SetIsMoveable(const bool& m);

	RigidBody* GetRigidBody();

	void Read(const string resourcename) override;
	void ReadParams(const string params) override;

	bool isSphere;
protected:
	bool moveable;
	RigidBody rigidBody;
};


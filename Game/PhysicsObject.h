#pragma once

#include "../NCLGL/Vector3.h"
#include "../NCLGL/Mesh.h"
#include "../NCLGL/Renderer.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/PhysicsEngine.h"
#include "../Physics/RigidBody.h"
#include "../ResourceManagment/ResourceBase.h"
#include "GameObject.h"

#include <algorithm>

/*
  Wrapper for physics entity.
   - links a scene node to the entity.
   Therefore ny changes applied to the physics entity
   apply to to the scenenode.
*/
class PhysicsObject : public GameObject
{
public:
	//Default physics entity
	PhysicsObject(Renderer* r, PhysicsEngine* p,
		bool isStatic, bool isSphere);

	//Custom rigid body
	PhysicsObject(Renderer* r, PhysicsEngine* p,
		bool isStatic, bool isSphere,
		RigidBody* rb);

	PhysicsObject();

	~PhysicsObject() {}

	//Adds to the physics engine
	void UpdatePhysics(PhysicsEngine* p)
	{
		p->AddRigidBody(&rigidBody);
	}

	void SetEntity(RigidBody* rb, Renderer* renderer, PhysicsEngine* physics);

	inline void ApplyForce(const Vector3& newtons)
	{
		rigidBody.ApplyForce(newtons);
	}

	Vector3 GetPosition() const
	{
		return position;
	}

	void SetPosition(const Vector3& pos)
	{
		position = pos;
		rigidBody.UpdatePosition(position);
	}

	Vector3 GetSize() const
	{
		return scale;
	}

	void SetSize(const Vector3& size);

	bool IsMoveable() const
	{
		return moveable;
	}

	 void SetIsMoveable(const bool& m)
	{
		moveable = m;
		rigidBody.isStatic = m;
	}

	RigidBody* GetRigidBody()
	{
		return &rigidBody;
	}

	bool		isSphere;

	virtual void Read(string resourcename);
	virtual void ReadParams(string params);

protected:
	bool		moveable;
	RigidBody	rigidBody;
};


#pragma once

#include "../NCLGL/Vector3.h"
#include "../NCLGL/Mesh.h"
#include "../NCLGL/Renderer.h"
#include "../Physics/SphereCollider.h"
#include "../Physics/PhysicsEngine.h"
#include "../Physics/RigidBody.h"
#include "../ResourceManagment/Resource.h"
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
	PhysicsObject(Renderer* r, PhysicsEngine* p,
		bool isStatic, bool isSphere);

	PhysicsObject(Renderer* r, PhysicsEngine* p,
		bool isStatic, bool isSphere,
		RigidBody* rb);

	PhysicsObject();

	~PhysicsObject() {}

	void SetEntity(RigidBody* rb, Renderer* renderer, PhysicsEngine* physics);
	void AddToPhysicsEngine(PhysicsEngine* p);

	inline void ApplyForce(const Vector3& newtons)
	{
		rigidBody.ApplyForce(newtons);
	}

	Vector3 GetPosition() const;
	void SetPosition(const Vector3& pos);

	Vector3 GetSize() const;
	void SetSize(const Vector3& size);

	bool IsMoveable() const;
	void SetIsMoveable(const bool& m);

	RigidBody* GetRigidBody();

	virtual void Read(string resourcename);
	virtual void ReadParams(string params);

	bool isSphere;
protected:
	bool moveable;
	RigidBody rigidBody;
};


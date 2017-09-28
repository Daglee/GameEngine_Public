#pragma once
//SEE PHYSICSENGINE.H COMMENT

#include "../nclgl/Maths/Vector3.h"
#include "../nclgl/Meshes/SceneNode.h"

class Collider;

class RigidBody
{
public:
	RigidBody(const Vector3 acceleration = Vector3(), const Vector3 velocity = Vector3(), const float mass = 1,
	          const float drag = 0.99f, const float gravity = -0.00001f, const string tag = "");
	virtual ~RigidBody();

	void ApplyForce(const Vector3& momentum);
	void UpdatePosition(const Vector3& pos);

	inline bool Ignores(const std::string rigidBodyTag) const
	{
		return ignoreTag == rigidBodyTag;
	}

	Vector3 acceleration;
	Vector3 velocity;
	Vector3 lastPosition;

	Collider* collider;
	SceneNode* parentMesh;

	float gravity;
	float drag;
	float inverseMass;
	bool isStatic;
	bool atRest;

	int restFrames = 0;

	string tag;
	string secondarytag = "";
	string ignoreTag = "";

	bool initialised;

	void UpdateMass(const float newMass)
	{
		mass = newMass;
		inverseMass = 1 / newMass;
	}

private:
	float mass;
};

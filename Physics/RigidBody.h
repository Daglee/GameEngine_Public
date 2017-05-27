#pragma once
//SEE PHYSICSENGINE.H COMMENT

#include "../nclgl/Vector3.h"
#include "../nclgl/SceneNode.h"

class Collider;

class RigidBody
{
public:
	RigidBody(Vector3 acceleration = Vector3(), Vector3 velocity = Vector3(), float mass = 1,
		float drag = 0.99f, float gravity = -0.00001f, std::string tag = "");
	virtual ~RigidBody();

	void ApplyForce(Vector3 momentum);
	void UpdatePosition	(Vector3 pos);

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
	
	std::string tag;
	std::string secondarytag = "";
	std::string ignoreTag = "";

	bool initialised;

	void UpdateMass(float m) 
	{
		mass = m;
		inverseMass = 1 / m;
	}

private:
	float mass;
};


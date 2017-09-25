#pragma once

class RigidBody;
class Vector3;

struct CollisionPair
{
	RigidBody* r1;
	RigidBody* r2;

	CollisionPair()
	{
		r1 = nullptr;
		r2 = nullptr;
	}

	CollisionPair(RigidBody* r1, RigidBody* r2)
	{
		this->r1 = r1;
		this->r2 = r2;
	}
};

class CollisionResponse
{
public:
	static void ImpulseResponse(CollisionPair& collisionPair, const Vector3& contactNormal,
		const float& penetrationDepth);
	
private:
	static void ProjectionResponse(CollisionPair& collisionPair, const Vector3& contactNormal,
		const float& penetrationDepth, const float& aInverseMass, const float& bInverseMass);
};


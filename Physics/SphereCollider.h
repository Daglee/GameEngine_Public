#pragma once
//SEE PHYSICSENGINE.H COMMENT

#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider(float radius)
	{
		this->radius = radius;
	}

	~SphereCollider() {}

	virtual bool IsColliding(Vector3& contactNormal, Collider& collider, float& penetrationDepth);
	virtual bool IsColliding(Vector3& contactNormal, SphereCollider& collider, float& penetrationDepth);
	virtual bool IsColliding(Vector3& contactNormal, PlaneCollider& collider, float& penetrationDepth);

	virtual Bounds ProjectOnAxis(Vector3& axis);

	float radius;
};


#pragma once
//SEE PHYSICSENGINE.H COMMENT

#include "Collider.h"

class SphereCollider : public Collider
{
public:
	explicit SphereCollider(const float radius)
	{
		this->radius = radius;
	}

	~SphereCollider()
	{
	}

	bool IsColliding(Vector3& contactNormal, Collider& collider, float& penetrationDepth) override;
	bool IsColliding(Vector3& contactNormal, SphereCollider& collider, float& penetrationDepth) override;
	bool IsColliding(Vector3& contactNormal, PlaneCollider& collider, float& penetrationDepth) override;

	Bounds ProjectOnAxis(const Vector3& axis) override;

	float radius;
};

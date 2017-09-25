#pragma once
//SEE PHYSICSENGINE.H COMMENT

#include "Collider.h"

class PlaneCollider : public Collider
{
public:
	explicit PlaneCollider(const Vector3 normal)
	{
		this->normal = normal;
	}

	~PlaneCollider()
	{
	}

	bool IsColliding(Vector3& contactNormal, Collider& collider, float& penetrationDepth) override;
	bool IsColliding(Vector3& contactNormal, SphereCollider& collider, float& penetrationDepth) override;
	bool IsColliding(Vector3& contactNormal, PlaneCollider& collider, float& penetrationDepth) override;

	Bounds ProjectOnAxis(const Vector3& axis) override;

	Vector3 normal;
};

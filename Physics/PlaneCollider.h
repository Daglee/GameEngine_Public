#pragma once
#include "Collider.h"

class PlaneCollider : public Collider
{
public:
	PlaneCollider(Vector3 normal) {
		this->normal = normal;
	}

	~PlaneCollider() {}

	virtual bool IsColliding(Vector3& contactNormal, Collider& collider, float& penetrationDepth);
	virtual bool IsColliding(Vector3& contactNormal, SphereCollider& collider, float& penetrationDepth);
	virtual bool IsColliding(Vector3& contactNormal, PlaneCollider& collider, float& penetrationDepth);
	virtual Bounds ProjectOnAxis(Vector3& axis);

	Vector3 normal;
};

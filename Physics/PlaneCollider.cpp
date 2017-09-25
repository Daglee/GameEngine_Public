#include "PlaneCollider.h"

#define INF

bool PlaneCollider::IsColliding(Vector3& contactNormal, Collider& collider, float& penetrationDepth)
{
	return collider.IsColliding(contactNormal, *this, penetrationDepth);
}

bool PlaneCollider::IsColliding(Vector3& contactNormal, SphereCollider& collider, float& penetrationDepth)
{
	return PlaneSphereCollision(collider, *this, contactNormal, penetrationDepth);
}

bool PlaneCollider::IsColliding(Vector3& contactNormal, PlaneCollider& collider, float& penetrationDepth)
{
	return false;
}

Bounds PlaneCollider::ProjectOnAxis(const Vector3& axis)
{
	Bounds b;

	const float m = std::numeric_limits<float>::max INF();
	b.min = -m;
	b.max = m;

	objbounds = b;
	return b;
}

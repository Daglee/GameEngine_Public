#include "SphereCollider.h"

bool SphereCollider::IsColliding(Vector3& contactNormal, Collider& collider, float& penetrationDepth)
{
	return collider.IsColliding(contactNormal, *this, penetrationDepth);
}

bool SphereCollider::IsColliding(Vector3& contactNormal, SphereCollider& collider, float& penetrationDepth)
{
	return SphereSphereCollision(*this, collider, contactNormal, penetrationDepth);
}

bool SphereCollider::IsColliding(Vector3& contactNormal, PlaneCollider& collider, float& penetrationDepth)
{
	return PlaneSphereCollision(*this, collider, contactNormal, penetrationDepth);
}

Bounds SphereCollider::ProjectOnAxis(const Vector3& axis)
{
	const float dot = Vector3::Dot(axis, position);

	Bounds b;
	b.max = dot + radius;
	b.min = dot - radius;
	objbounds = b;

	return b;
}

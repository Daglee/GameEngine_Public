#include "Collider.h"

#include "SphereCollider.h"
#include "PlaneCollider.h"

bool Collider::SphereSphereCollision(SphereCollider& a,
	SphereCollider& b, Vector3& contactNormal, float& penetrationDepth)
{
	float combinedRadii = a.radius + b.radius;

	Vector3 distance = a.position - b.position;

	float length = (distance).Length();

	if (length < combinedRadii)
	{
		penetrationDepth = combinedRadii - length;

		distance.Normalise();
		contactNormal = distance;

		return true;
	}
	else
	{
		return false;
	}
}

bool Collider::PlaneSphereCollision(SphereCollider& sphere,
	PlaneCollider& plane, Vector3& contactNormal, float& penetrationDepth)
{
	Vector3 normal = plane.normal;

	float dot = -(Vector3::Dot(plane.position, normal));
	float length = Vector3::Dot(normal, sphere.position + plane.position);
	float distance = Vector3::Dot(normal, sphere.position) + plane.position.Length();

	if (distance < sphere.radius)
	{
		if (sphere.position.x - sphere.radius > plane.position.x + plane.parentsize.x
			|| sphere.position.x - sphere.radius < plane.position.x - plane.parentsize.x
			|| sphere.position.z < plane.position.z - plane.parentsize.z
			|| sphere.position.z >= plane.position.z + plane.parentsize.z)
		{
			return false;
		}
		else
		{
			penetrationDepth = (sphere.radius - distance);

			normal.Normalise();
			contactNormal = normal;

			return true;
		}
	}
	else
	{
		return false;
	}
}
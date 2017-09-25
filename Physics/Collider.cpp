#include "Collider.h"

#include "SphereCollider.h"
#include "PlaneCollider.h"

bool Collider::SphereSphereCollision(SphereCollider& a,
                                     SphereCollider& b, Vector3& contactNormal, float& penetrationDepth)
{
	const float combinedRadii = a.radius + b.radius;

	Vector3 distance = a.position - b.position;

	const float length = (distance).Length();

	const bool collided = length < combinedRadii;

	if (collided)
	{
		penetrationDepth = combinedRadii - length;

		distance.Normalise();
		contactNormal = distance;
	}

	return collided;
}

bool Collider::PlaneSphereCollision(SphereCollider& sphere,
                                    PlaneCollider& plane, Vector3& contactNormal, float& penetrationDepth)
{
	float dot = -(Vector3::Dot(plane.position, plane.normal));
	float length = Vector3::Dot(plane.normal, sphere.position + plane.position);
	const float distance = Vector3::Dot(plane.normal, sphere.position) + plane.position.Length();

	if (distance < sphere.radius)
	{
		if (sphere.position.x - sphere.radius > plane.position.x + plane.parentsize.x
			|| sphere.position.x - sphere.radius < plane.position.x - plane.parentsize.x
			|| sphere.position.z < plane.position.z - plane.parentsize.z
			|| sphere.position.z >= plane.position.z + plane.parentsize.z)
		{
			return false;
		}

		penetrationDepth = (sphere.radius - distance);

		Vector3 normal = plane.normal;
		normal.Normalise();
		contactNormal = normal;

		return true;
	}

	return false;
}

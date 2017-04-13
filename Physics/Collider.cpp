#include "Collider.h"

#include "SphereCollider.h"
#include "PlaneCollider.h"

bool Collider::SphereSphereCollision(SphereCollider& a,
	SphereCollider& b, Vector3& contactNormal, float& penetrationDepth) {
	float d = a.radius + b.radius;

	Vector3 norm = a.position - b.position;
	float dist = (norm).Length();

	if (dist < d) {
		penetrationDepth = (a.radius + b.radius) - dist;

		norm.Normalise();
		contactNormal = norm;

		return true;
	}
	else return false;
}

bool Collider::PlaneSphereCollision(SphereCollider& sphere,
	PlaneCollider& plane, Vector3& contactNormal, float& penetrationDepth) {
	Vector3 norm = plane.normal;
	float dist = Vector3::Dot(norm, sphere.position) + plane.position.Length();

	if (dist < sphere.radius) {
		if (sphere.position.x > plane.position.x + plane.parentsize.x
			|| sphere.position.z > plane.position.z + plane.parentsize.z
			|| sphere.position.x < plane.position.x - plane.parentsize.x
			|| sphere.position.z < plane.position.z - plane.parentsize.z) {
			return false;
		}
		else {
			penetrationDepth = (sphere.radius - dist);

			norm.Normalise();
			contactNormal = norm;

			return true;
		}
	} 
	else return false;
}
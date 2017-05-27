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

	float dot = -(Vector3::Dot(plane.position, norm));
	float length = Vector3::Dot(norm, sphere.position + plane.position);
	float dist = Vector3::Dot(norm, sphere.position) + plane.position.Length();
	//if (norm.z != 0) dist = dist - (plane.parentsize.z * 2 * norm.z) - (sphere.radius * norm.z);
	if (dist < sphere.radius) {
		if (sphere.position.x - sphere.radius> plane.position.x + plane.parentsize.x
			|| sphere.position.x - sphere.radius< plane.position.x - plane.parentsize.x
			|| sphere.position.z/* + sphere.radius*/< plane.position.z - plane.parentsize.z
			|| sphere.position.z/* - sphere.radius*/>= plane.position.z + plane.parentsize.z) {
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

	//float project =/* std::abs(*/sphere.position.x * norm.x +
	//	sphere.position.y * norm.y +
	//	sphere.position.z * norm.z/* +plane.position.Length()*/;
	//if (plane.position.z > 0) {
	//	project = project + plane.position.Length();
	//}
	//else project = project + plane.position.Length();

	//project = std::abs(project);
	//	

	////float project = std::abs(Vector3::Dot(norm, sphere.position) +
	////	plane.position.Length());

	//if (project < sphere.radius) {
	//	if (norm.y != 0) {
	//		if (sphere.position.x - sphere.radius > plane.position.x + plane.parentsize.x
	//			|| sphere.position.x - sphere.radius < plane.position.x - plane.parentsize.x
	//			|| sphere.position.z - sphere.radius > plane.position.z + plane.parentsize.z
	//			|| sphere.position.z - sphere.radius < plane.position.z - plane.parentsize.z) {
	//			return false;
	//			}
	//	}
	//	if (norm.z != 0) {
	//		if (sphere.position.x - sphere.radius > plane.position.x + plane.parentsize.x
	//			|| sphere.position.x - sphere.radius < plane.position.x - plane.parentsize.x) {
	//			return false;
	//		}
	//		//if (sphere.position.z > plane.position.z * norm.z) return false;
	//		//else dist = dist - plane.parentsize.z - sphere.radius;
	//	}
	//	if (norm.x != 0) {
	//		if (sphere.position.z - sphere.radius > plane.position.z + plane.parentsize.z
	//			|| sphere.position.z - sphere.radius < plane.position.z - plane.parentsize.z) {
	//			return false;
	//		}
	//	}
	//	penetrationDepth = (sphere.radius - project + (plane.parentsize.z * norm.z));

	//	norm.Normalise();
	//	contactNormal = norm;

	//	return true;
	//}
	//else return false;
}
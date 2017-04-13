#pragma once

#include "../../nclgl/Vector3.h"
#include "../../nclgl/Matrix4.h"

class SphereCollider;
class PlaneCollider;

//For sort and sweep
struct Bounds
{
	float min, max;
};

class Collider
{
public:
	Collider() {};
	virtual ~Collider() {};

	virtual bool IsColliding(Vector3& contactNormal, Collider& collider,		float& penetrationDepth) = 0;
	virtual bool IsColliding(Vector3& contactNormal, SphereCollider& collider,	float& penetrationDepth) = 0;
	virtual bool IsColliding(Vector3& contactNormal, PlaneCollider& collider,	float& penetrationDepth) = 0;

	bool SphereSphereCollision(SphereCollider& a,
		SphereCollider& b, Vector3& contactNormal, float& penetrationDepth);

	bool PlaneSphereCollision(SphereCollider& sphere,
		PlaneCollider& plane, Vector3& contactNormal, float& penetrationDepth);

	virtual Bounds ProjectOnAxis(Vector3& axis) = 0;

	Vector3	position;
	Vector3 parentsize;
	Bounds	objbounds;
};

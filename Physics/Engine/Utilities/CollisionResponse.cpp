#include "CollisionResponse.h"

#include "../../Entities/RigidBody.h"
#include "../../Colliders/Collider.h"
#include "../nclgl/Maths/Vector3.h"

const float MAX_AT_REST_VELOCITY = 0.001f;

void CollisionResponse::ImpulseResponse(CollisionPair& collisionPair, const Vector3& contactNormal,
	const float& penetrationDepth)
{
	const Vector3 vab = collisionPair.r1->velocity - collisionPair.r2->velocity;

	//Reciprocals of masses
	const float aMass = collisionPair.r1->isStatic ? 0 : collisionPair.r1->inverseMass;
	const float bMass = collisionPair.r2->isStatic ? 0 : collisionPair.r2->inverseMass;

	const float e = 0.999f;

	const float J = (-(1.0f + e) * Vector3::Dot(vab, contactNormal)) /
		(Vector3::Dot(contactNormal, contactNormal * (aMass + bMass)));

	//Fix the overlap first
	ProjectionResponse(collisionPair, contactNormal, penetrationDepth, aMass, bMass);

	//No send them flying apart (maybe)...
	Vector3 aVelocity = (contactNormal * (J * aMass));
	Vector3 bVelocity = (contactNormal * (J * bMass));

	collisionPair.r1->velocity += aVelocity;
	collisionPair.r2->velocity -= bVelocity;

	//No longer at rest?
	if (aVelocity.sqrLength() > MAX_AT_REST_VELOCITY)
	{
		collisionPair.r1->atRest = false;
	}

	if (bVelocity.sqrLength() > MAX_AT_REST_VELOCITY)
	{
		collisionPair.r2->atRest = false;
	}
}

void CollisionResponse::ProjectionResponse(CollisionPair& collisionPair, const Vector3& contactNormal,
	const float& penetrationDepth, const float& aInverseMass, const float& bInverseMass)
{
	const float moveRatio = aInverseMass / (aInverseMass + bInverseMass);

	const Vector3 xPosition = collisionPair.r1->collider->position;
	const Vector3 xOffset = xPosition - (contactNormal * penetrationDepth * moveRatio);

	const Vector3 yPosition = collisionPair.r2->collider->position;
	const Vector3 yOffset = yPosition + (contactNormal * penetrationDepth * (1.0f - moveRatio));

	collisionPair.r1->UpdatePosition(xOffset);
	collisionPair.r2->UpdatePosition(yOffset);
}

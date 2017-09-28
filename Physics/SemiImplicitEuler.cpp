#include "SemiImplicitEuler.h"

#include "RigidBody.h"
#include "../nclgl/Maths/Vector3.h"
#include "Collider.h"

const int FRAMES_UNTIL_AT_REST = 10;
const float MAX_AT_REST_VELOCITY = 0.001f;

void SemiImplicitEuler::UpdateRigidBody(RigidBody& rigidBody, const Vector3& gravity, const float& deltaTime)
{
	rigidBody.velocity = (rigidBody.velocity + ((rigidBody.acceleration + gravity) * deltaTime)) * rigidBody.drag;

	const Vector3 displacement = rigidBody.collider->position + (rigidBody.velocity * deltaTime);

	if (((displacement - rigidBody.lastPosition) / deltaTime).sqrLength() < MAX_AT_REST_VELOCITY)
	{
		++rigidBody.restFrames;

		if (rigidBody.restFrames > FRAMES_UNTIL_AT_REST)
		{
			return;
		}
	}
	else
	{
		rigidBody.restFrames = 0;
	}

	//if the object is not at rest, then update it's position and last displacement
	rigidBody.UpdatePosition(displacement);
	rigidBody.lastPosition = rigidBody.collider->position;
}

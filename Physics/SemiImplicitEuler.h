#pragma once

class RigidBody;
class Vector3;

class SemiImplicitEuler
{
public:
	static void UpdateRigidBody(RigidBody& rigidBody, const Vector3& gravity, const float& deltaTime);
};


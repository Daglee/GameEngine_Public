#pragma once

#include "PlayerController.h"

#include "../nclgl/Matrix4.h"

#include <string>


class TopDownController : public PlayerController
{
public:
	TopDownController(InputMapper* input, CharacterModel* playerModel = nullptr, RigidBody* rigidBody = nullptr);
	~TopDownController() {}

	void SetSpeedMultiplier(int multiplier)
	{
		speedMultiplier = multiplier;
	}

protected:
	void ApplyRotation();
	void UpdatePosition();

private:
	bool MovementInputted(const Vector3& movement);

	int speedMultiplier;
};


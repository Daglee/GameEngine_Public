#pragma once

#include "PlayerController.h"

class TopDownController : public PlayerController
{
public:
	TopDownController(InputMapper* input, CharacterModel* playerModel = nullptr, RigidBody* rigidBody = nullptr);
	~TopDownController() {}

	void SetSpeedMultiplier(const int multiplier)
	{
		speedMultiplier = multiplier;
	}

protected:
	void ApplyRotation() override;
	void UpdatePosition() override;

private:
	bool MovementInputted(const Vector3& movement);

	int speedMultiplier;
};


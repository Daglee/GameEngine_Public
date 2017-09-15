#pragma once

#include "../UISystem/InputMapper.h"

#include "CharacterModel.h"
#include "../Physics/RigidBody.h"

class PlayerController
{
public:
	PlayerController(InputMapper* input) : input(input) {}
	virtual ~PlayerController() {}

	void ApplyInputs();

	const Matrix4 GetCurrentRotation() const;
	InputMapper* GetInputMapper() const;

	void SetInputMapper(InputMapper* inputMapper);
	void SetCharacterModel(CharacterModel* newModel);
	void SetRigidBody(RigidBody* newRigidBody);
	void SetMovementSound(std::string newMovementSound);

protected:
	virtual void ApplyRotation() = 0;
	virtual void UpdatePosition() = 0;

	InputMapper* input;

	CharacterModel* playerModel;
	RigidBody* rigidBody;

	Matrix4 currentRotation;
	std::string movementSound;
};


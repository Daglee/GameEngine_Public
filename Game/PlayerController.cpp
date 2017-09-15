#include "PlayerController.h"

void PlayerController::ApplyInputs()
{
	ApplyRotation();
	UpdatePosition();
}

const Matrix4 PlayerController::GetCurrentRotation() const
{
	return currentRotation;
}

InputMapper* PlayerController::GetInputMapper() const
{
	return input;
}

void PlayerController::SetInputMapper(InputMapper* inputMapper)
{
	input = inputMapper;
}

void PlayerController::SetCharacterModel(CharacterModel* newModel)
{
	playerModel = newModel;
}

void PlayerController::SetRigidBody(RigidBody* newRigidBody)
{
	rigidBody = newRigidBody;
}

void PlayerController::SetMovementSound(std::string newMovementSound)
{
	this->movementSound = newMovementSound;
}
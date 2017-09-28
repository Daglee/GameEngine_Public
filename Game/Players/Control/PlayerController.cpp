#include "PlayerController.h"

#include "../Player.h"
#include "../Model/CharacterModel.h"
#include "../Physics/RigidBody.h"

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

void PlayerController::UpdatePropertiesFromPlayer(Player* player)
{
	SetCharacterModel(player->GetPlayerModel());
	SetRigidBody(player->GetRigidBody());
	SetMovementSound(player->walkingSoundName);
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

void PlayerController::SetMovementSound(const string newMovementSound)
{
	this->movementSound = newMovementSound;
}
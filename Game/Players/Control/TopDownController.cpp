#include "TopDownController.h"

#include "../Model/CharacterModel.h"
#include "../Physics/Entities/RigidBody.h"
#include "../GameLogicFSM/Messaging/MessageSystem.h"
#include "../../Audio/AudioManager.h"
#include "../../../ResourceManagement/Utilities/StringUtility.h"

const Vector3 Y_ROTATION_AXIS = Vector3(0, 1, 0);

TopDownController::TopDownController(InputMapper* input, CharacterModel* playerModel, RigidBody* rigidBody)
	: PlayerController(input)
{
	this->playerModel = playerModel;
	this->rigidBody = rigidBody;

	speedMultiplier = 30;
}

void TopDownController::ApplyRotation()
{
	const float inputtedRotationInDegrees = input->GetRotation();

	const Matrix4 rotation = Matrix4::Rotation(inputtedRotationInDegrees, Y_ROTATION_AXIS);
	playerModel->SetTransform(playerModel->GetTransform() * rotation);

	if (inputtedRotationInDegrees != 0)
	{
		currentRotation = rotation;
		MessageSystem::GetInstance()->TransmitMessage(StringUtility::Hash("player_rotated"));
	}
}

void TopDownController::UpdatePosition()
{
	const Vector3 movement = input->GetMovement() * static_cast<float>(speedMultiplier);

	rigidBody->ApplyForce(movement);

	if (MovementInputted(movement))
	{
		AudioManager::GetInstance()->BeginPlay(movementSound);

		MessageSystem::GetInstance()->TransmitMessage(StringUtility::Hash("player_moved"));
		MessageSystem::GetInstance()->BeginEvent(StringUtility::Hash(rigidBody->tag + "moving"));
	}
	else
	{
		AudioManager::GetInstance()->StopPlay(movementSound);
	}
}

bool TopDownController::MovementInputted(const Vector3& movement)
{
	return (movement.x != 0 || movement.y != 0 || movement.z != 0);
}

#pragma once

#include "../UISystem/Input Managment/Input/InputMapper.h"
#include "../nclgl/Maths/Matrix4.h"

#include <string>

class RigidBody;
class CharacterModel;
class Player;

class PlayerController
{
public:
	explicit PlayerController(InputMapper* input) : input(input)
	{
		playerModel = nullptr;
		rigidBody = nullptr;
	}

	virtual ~PlayerController() {}

	void ApplyInputs();

	const Matrix4 GetCurrentRotation() const;
	InputMapper* GetInputMapper() const;

	void UpdatePropertiesFromPlayer(Player* player);

	void SetInputMapper(InputMapper* inputMapper);
	void SetCharacterModel(CharacterModel* newModel);
	void SetRigidBody(RigidBody* newRigidBody);
	void SetMovementSound(const std::string newMovementSound);

protected:
	virtual void ApplyRotation() = 0;
	virtual void UpdatePosition() = 0;

	InputMapper* input;

	CharacterModel* playerModel;
	RigidBody* rigidBody;

	Matrix4 currentRotation;
	std::string movementSound;
};


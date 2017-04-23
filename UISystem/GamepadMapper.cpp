#include "GamepadMapper.h"

//Just for clearer array indexes
#define MOVEMENT_X	0
#define MOVEMENT_Z	1
#define ROTATION_X	3
#define ROTATION_Y	2
#define FIRED		4

GamepadMapper::GamepadMapper() : InputMapper()
{
	rawRotation = Vector3(-1, 0, 0);
}

void GamepadMapper::FillInputs()
{
	gamepad->Update();

	/*
	No checks are needed for whether the controller is still
	connected. InputManager handles that with polling functions.
	*/

	// 0 - 1 = left stick
	if (!gamepad->LStickInDeadzone()) {
		inputs[MOVEMENT_X] = gamepad->LeftStick_X();
		inputs[MOVEMENT_Z] = gamepad->LeftStick_Y();
	}

	// 2 - 3 = right stick
	if (!gamepad->RStickInDeadzone()) {
		inputs[ROTATION_Y] = gamepad->RightStick_X();
		inputs[ROTATION_X] = gamepad->RightStick_Y();
	}

	// 4 -5 = triggers
	inputs[5]		= gamepad->LeftTrigger();
	inputs[FIRED]	= gamepad->RightTrigger();
}

void GamepadMapper::ClearInputs()
{
	for (int i = 0; i < 6; i++) {
		inputs[i] = 0;
	}
}

//Y Movement is not needed for a top down shooter.
Vector3 GamepadMapper::GetMovement()
{
	Vector3 pos(inputs[MOVEMENT_X], 0, -inputs[MOVEMENT_Z]);

	return pos;
}

float GamepadMapper::GetRotation()
{
	if (inputs[ROTATION_Y] != 0 || inputs[ROTATION_X] != 0) {
		float rotation = atan2(inputs[ROTATION_X], inputs[ROTATION_Y])
			* (float)(180.0f / M_PI) + 90.0f;

		//So we dont keep bouncing back and forth
		if (rotation < 0) {
			rotation = 360 - (-rotation);
		}

		//To stop the player from spinning like mad
		float newRotation = rotation - prevRotation;
		prevRotation = rotation;

		//The x and y values before being converted to degrees.
		rawRotation = Vector3((inputs[ROTATION_Y]), 0, (inputs[ROTATION_X]));

		return newRotation;
	}
	else return 0;
}

bool GamepadMapper::Fired()
{
	return inputs[FIRED] > 0; //Could set a limit for the trigger here...
}
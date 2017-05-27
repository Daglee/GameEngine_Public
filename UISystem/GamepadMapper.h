#pragma once

#include "InputMapper.h"
#include "../nclgl/Gamepad.h"
#include "../nclgl/Vector3.h"

/*
  Translates inputs from the controller into inputs for a game object.
  (takes inputs and prepares them for game logic, etc)
  The inputs are retrieved from this classes Gamepad instance.
*/
class GamepadMapper : public InputMapper
{
public:
	GamepadMapper();
	~GamepadMapper() {}

	//Buffer functions
	void FillInputs();
	void ClearInputs();

	//Left stick
	Vector3 GetMovement();

	//Right stick
	float GetRotation();

	/*
	  Left/Right Triggers.
	  Could be a float value but it's not required
	  in this scenario.
	*/
	bool Fired();
	bool Reload();

	void SetGamePad(Gamepad* gp) 
	{
		gamepad = gp;
		id = gamepad->gamepadIndex + 2;
	}

private:
	int id;
	Gamepad* gamepad;
	float inputs[6];
};


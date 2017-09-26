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
	void FillInputs() override;
	void ClearInputs() override;

	//Left stick
	Vector3 GetMovement() override;

	//Right stick
	float GetRotation() override;

	/*
	  Left/Right Triggers.
	  Could be a float value but it's not required
	  in this scenario.
	*/
	bool Fired() override;
	bool Reload() override;

	void SetGamePad(Gamepad* newGamepad)
	{
		gamepad = newGamepad;
		id = gamepad->gamepadIndex + 2;
	}

private:
	void AnnounceTeamChanges() const;

	enum class BASIC_CONTROL;
	enum class RHS_BUTTONS;
	int id;
	Gamepad* gamepad;
	float inputs[6];
};


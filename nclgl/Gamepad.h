#pragma once

#include <windows.h>
#include <Xinput.h>
#include "../ResourceManagment/ResourceBase.h"

class Gamepad : public ResourceBase
{
public:
	Gamepad();
	Gamepad(int index);
	~Gamepad();

	void Update();

	//Thumbsticks
	//Return true if stick is in deadzone
	bool LStickInDeadzone();
	bool RStickInDeadzone();

	//Return axis values
	float LeftStick_X();
	float LeftStick_Y();
	float RightStick_X();
	float RightStick_Y();

	// Trigger functions - Return value of trigger
	float LeftTrigger();
	float RightTrigger();

	//Utility functions
	XINPUT_STATE GetState();
	int	 GetIndex();
	void SetIndex(int index);
	bool Connected();

	void Read(string resourcename);
	void ReadParams(string params);

	// Vibrate the gamepad (0.0f none, 1.0f max speed)
	void Rumble(float a_fLeftMotor = 0.0f, float a_fRightMotor = 0.0f);

	int	gamepadIndex; //eg 1, 2, 3, 4
private:
	XINPUT_STATE state; //Current gamepad state
};

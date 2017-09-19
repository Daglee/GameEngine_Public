#include "Gamepad.h"

#pragma comment(lib, "Xinput.lib")

//Define the 'XButtonIDs' struct as 'XButtons'
XButtonIDs XButtons;

//'XButtonIDs' Default constructor
XButtonIDs::XButtonIDs()
{
	// These values are used to index the XINPUT_Buttons array,
	// accessing the matching XINPUT button value

	A = 0;
	B = 1;
	X = 2;
	Y = 3;

	DPadUp = 4;
	DPadDown = 5;
	DPadLeft = 6;
	DPadRight = 7;

	LShoulder = 8;
	RShoulder = 9;

	LThumbstick = 10;
	RThumbstick = 11;

	Start = 12;
	Back = 13;
}

Gamepad::Gamepad() : ResourceBase()
{
	//Iterate through all gamepad buttons
	for (int i = 0; i < ButtonCount; i++)
	{
		bPrev_ButtonStates[i] = false;
		bButtonStates[i] = false;
		bGamepad_ButtonsDown[i] = false;
	}

	this->SetResourceSize(sizeof(*this));
}

Gamepad::Gamepad(int index) : ResourceBase()
{
	//index or 1, 2, 3 etc matches to 0, 1, 2 etc
	gamepadIndex = index - 1;

	//Iterate through all gamepad buttons
	for (int i = 0; i < ButtonCount; i++)
	{
		bPrev_ButtonStates[i] = false;
		bButtonStates[i] = false;
		bGamepad_ButtonsDown[i] = false;
	}

	this->SetResourceSize(sizeof(*this));
}

Gamepad::~Gamepad()
{}

//Update gamepad state
void Gamepad::Update()
{
	state = GetState(); //Obtain current gamepad state

	// Iterate through all gamepad buttons
	for (int i = 0; i < ButtonCount; i++)
	{
		//Set button state for current frame
		bButtonStates[i] = (state.Gamepad.wButtons &
			XINPUT_Buttons[i]) == XINPUT_Buttons[i];

		//Set 'DOWN' state for current frame
		bGamepad_ButtonsDown[i] = !bPrev_ButtonStates[i] &&
			bButtonStates[i];
	}
}

//Update button states for next frame
void Gamepad::RefreshState()
{
	memcpy(bPrev_ButtonStates, bButtonStates,
		sizeof(bPrev_ButtonStates));
}

//Deadzone check for Left Thumbstick
bool Gamepad::LStickInDeadzone()
{
	//Obtain the X & Y axes of the stick
	short sX = state.Gamepad.sThumbLX;
	short sY = state.Gamepad.sThumbLY;

	//X axis is outside of deadzone
	if (sX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		sX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return false;
	}

	//Y axis is outside of deadzone
	if (sY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		sY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return false;
	}

	//One (or both axes) axis is inside of deadzone
	return true;
}

//Deadzone check for Right Thumbstick
bool Gamepad::RStickInDeadzone()
{
	//Obtain the X & Y axes of the stick
	short sX = state.Gamepad.sThumbRX;
	short sY = state.Gamepad.sThumbRY;

	//X axis is outside of deadzone
	if (sX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
		sX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		return false;
	}

	//Y axis is outside of deadzone
	if (sY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
		sY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		return false;
	}

	//One (or both axes) axis is inside of deadzone
	return true;
}

float Gamepad::LeftStick_X()
{
	//Obtain X axis of left stick
	short sX = state.Gamepad.sThumbLX;

	//Return axis value, converted to a float
	return (static_cast<float>(sX) / 32768.0f);
}

float Gamepad::LeftStick_Y()
{
	//Obtain Y axis of left stick
	short sY = state.Gamepad.sThumbLY;

	//Return axis value, converted to a float
	return (static_cast<float>(sY) / 32768.0f);
}

float Gamepad::RightStick_X()
{
	//Obtain X axis of right stick
	short sX = state.Gamepad.sThumbRX;

	//Return axis value, converted to a float
	return (static_cast<float>(sX) / 32768.0f);
}

float Gamepad::RightStick_Y()
{
	//Obtain the Y axis of the left stick
	short sY = state.Gamepad.sThumbRY;

	//Return axis value, converted to a float
	return (static_cast<float>(sY) / 32768.0f);
}

float Gamepad::LeftTrigger()
{
	//Obtain value of left trigger
	BYTE Trigger = state.Gamepad.bLeftTrigger;

	if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		return Trigger / 255.0f;
	}
	else return 0.0f; //Trigger was not pressed
}

float Gamepad::RightTrigger()
{
	//Obtain value of right trigger
	BYTE Trigger = state.Gamepad.bRightTrigger;

	if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		return Trigger / 255.0f;
	}
	else return 0.0f; //Trigger was not pressed

}

//Return true if button is pressed, false if not
bool Gamepad::GetButtonPressed(int button)
{
	if (state.Gamepad.wButtons & XINPUT_Buttons[button])
	{
		return true; //The button is pressed
	}

	return false; //The button is not pressed
}

//Frame-specific version of 'GetButtonPressed' function
bool Gamepad::GetButtonDown(int button)
{
	return bGamepad_ButtonsDown[button];
}

//Vibrate the gamepad (values of 0.0f to 1.0f only)
void Gamepad::Rumble(float a_fLeftMotor, float a_fRightMotor)
{
	XINPUT_VIBRATION VibrationState;
	ZeroMemory(&VibrationState, sizeof(XINPUT_VIBRATION));

	//Calculate vibration values
	int iLeftMotor = int(a_fLeftMotor * 65535.0f);
	int iRightMotor = int(a_fRightMotor * 65535.0f);

	//Set vibration values
	VibrationState.wLeftMotorSpeed = iLeftMotor;
	VibrationState.wRightMotorSpeed = iRightMotor;

	//Set the vibration state
	XInputSetState(gamepadIndex, &VibrationState);
}

XINPUT_STATE Gamepad::GetState()
{
	//Temporary state to return
	XINPUT_STATE gamepadState;

	ZeroMemory(&gamepadState, sizeof(XINPUT_STATE));

	XInputGetState(gamepadIndex, &gamepadState);

	return gamepadState;
}

//Return gamepad index
int Gamepad::GetIndex()
{
	return gamepadIndex;
}

void Gamepad::SetIndex(int index)
{
	gamepadIndex = index;
}

//Return true if the gamepad is connected
bool Gamepad::Connected()
{
	//Zero memory
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	//Get the state of the gamepad
	DWORD Result = XInputGetState(gamepadIndex, &state);

	if (Result == ERROR_SUCCESS)
	{
		return true;  //The gamepad is connected
	}
	else return false; //The gamepad is not connected
}

void Gamepad::Read(string resourcename)
{
	string name = resourcename;
	string snum = name.substr(name.size() - 1, name.size());
	int id = atoi(snum.c_str());
	gamepadIndex = id;
	this->SetResourceName(resourcename);
}

void Gamepad::ReadParams(string params)
{
	Read(params);
}
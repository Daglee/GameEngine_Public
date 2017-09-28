#include "MKMapper.h"
#include "../GameLogicFSM/Messaging/MessageSystem.h"
#include <math.h>

enum BASIC_CONTROL
{
	MOVEMENT_X,
	MOVEMENT_Z,
	ROTATION_X,
	ROTATION_Y,
	FIRED,
	RELOAD
};

MKMapper::MKMapper(Window* w, string filename) : InputMapper()
{
	window = w;
	rawRotation = Vector3(-1, 0, 0);

	ReadMapping(filename);
}

void MKMapper::FillInputs()
{
	//Key: W - index: 1
	if (Window::GetKeyboard()->KeyDown(NEG_MOVEMENT_Z))
	{
		inputs[MOVEMENT_Z] = inputs[MOVEMENT_Z] - 1.1f;
	}

	//Key: A - index: 0
	if (Window::GetKeyboard()->KeyDown(NEG_MOVEMENT_X))
	{
		inputs[MOVEMENT_X] = inputs[MOVEMENT_X] - 1.1f;
	}

	//Key: S - index: 1
	if (Window::GetKeyboard()->KeyDown(POS_MOVEMENT_Z))
	{
		inputs[MOVEMENT_Z] = inputs[MOVEMENT_Z] + 1.1f;
	}

	//Key: D - index: 0
	if (Window::GetKeyboard()->KeyDown(POS_MOVEMENT_X))
	{
		inputs[MOVEMENT_X] = inputs[MOVEMENT_X] + 1.1f;
	}

	if (Window::GetKeyboard()->KeyDown(STOP_GAME))
	{
		MessageSystem::GetInstance()->BeginEvent(Log::Hash("stop_game"));
	}

	//Key: LC - index - 4
	if (Window::GetMouse()->ButtonDown(FIRE_BUTTON))
	{
		inputs[FIRED] = 1;
	}

	if (Window::GetKeyboard()->KeyDown(RELOAD_BUTTON))
	{
		inputs[RELOAD] = 1;
	}

	//Team changing buttons
	AnnounceTeamChanges();

	//Rotation - Y - 2 (pitch)
	inputs[ROTATION_Y] = Window::GetMouse()->GetRelativePosition().y / 10;

	//Rotation - X - 3 (yaw)
	inputs[ROTATION_X] = Window::GetMouse()->GetRelativePosition().x / 10;
}

void MKMapper::AnnounceTeamChanges() const
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_1))
	{
		MessageSystem::GetInstance()->TransmitMessage(Log::Hash("Player1SwitchRedTeam"));
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_2))
	{
		MessageSystem::GetInstance()->TransmitMessage(Log::Hash("Player1SwitchBlueTeam"));
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_3))
	{
		MessageSystem::GetInstance()->TransmitMessage(Log::Hash("Player1SwitchGreenTeam"));
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_4))
	{
		MessageSystem::GetInstance()->TransmitMessage(Log::Hash("Player1SwitchYellowTeam"));
	}
}

void MKMapper::ClearInputs()
{
	for (int i = 0; i < 6; i++)
	{
		inputs[i] = 0;
	}
}

Vector3 MKMapper::GetMovement()
{
	Vector3 pos(inputs[MOVEMENT_X], 0, inputs[MOVEMENT_Z]);

	return pos;
}

float MKMapper::GetRotation()
{
	float newRotation = 0;

	if (inputs[ROTATION_Y] != 0 || inputs[ROTATION_X] != 0)
	{
		float rotation = atan2(inputs[ROTATION_X], inputs[ROTATION_Y])
			* static_cast<float>(180.0 / M_PI);

		if (rotation < 0)
		{
			rotation = 360 - (-rotation);
		}

		//To stop the player from spinning like mad
		newRotation = rotation - prevRotation;
		prevRotation = rotation;

		rawRotation = Vector3(inputs[ROTATION_X] / 10, 0, -inputs[ROTATION_Y] / 10);
	}

	return newRotation;
}

bool MKMapper::Fired()
{
	return inputs[FIRED] != 0;
}

bool MKMapper::Reload()
{
	return inputs[RELOAD] != 0;
}

/*
  Example at: ../../ButtonMappings/MKMap.txt
*/
void MKMapper::ReadMapping(const string filename)
{
	ifstream file(filename);
	string button;

	getline(file, button);
	NEG_MOVEMENT_X = Window::GetKeyboard()->Mapper(button);

	getline(file, button);
	POS_MOVEMENT_X = Window::GetKeyboard()->Mapper(button);

	getline(file, button);
	POS_MOVEMENT_Z = Window::GetKeyboard()->Mapper(button);

	getline(file, button);
	NEG_MOVEMENT_Z = Window::GetKeyboard()->Mapper(button);

	getline(file, button);
	PAUSE = Window::GetKeyboard()->Mapper(button);

	getline(file, button);
	STOP_GAME = Window::GetKeyboard()->Mapper(button);

	getline(file, button);
	RELOAD_BUTTON = Window::GetKeyboard()->Mapper(button);
}
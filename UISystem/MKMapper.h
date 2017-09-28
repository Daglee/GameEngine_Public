#pragma once

#include "InputMapper.h"
#include "../nclgl/Rendering/View/Window.h"
#include "../nclgl/Maths/Vector3.h"

#include <string>

/*
  Translates inputs from the M&K into inputs for a game object.
  The inputs are retrieved directly from the game window's instance.
*/
class MKMapper : public InputMapper
{
public:
	MKMapper(Window* w, string filename);

	~MKMapper() {}

	//Fill and clear buffer
	void FillInputs() override;
	void ClearInputs() override;

	Vector3 GetMovement() override;
	float GetRotation() override;
	bool Fired() override;
	bool Reload() override;

	float* GetInputs()
	{
		return inputs;
	}

	//Read in the button mappings.
	virtual void ReadMapping(const string filename);

	KeyboardKeys PAUSE;
private:
	void AnnounceTeamChanges()  const;

	float inputs[6];
	Window* window;

	//Movement buttons can be read in from file.
	KeyboardKeys NEG_MOVEMENT_X;
	KeyboardKeys POS_MOVEMENT_X;
	KeyboardKeys POS_MOVEMENT_Z;
	KeyboardKeys NEG_MOVEMENT_Z;
	KeyboardKeys STOP_GAME;
	MouseButtons FIRE_BUTTON;
	KeyboardKeys RELOAD_BUTTON;
};
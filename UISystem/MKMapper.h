#pragma once

#include "InputMapper.h"
#include "../../nclgl/Window.h"
#include "../../nclgl/Vector3.h"

#include <string>
#include <unordered_map>

/*
  Translates inputs from the M&K into inputs for a game object.
  The inputs are retrieved directly from the game window's instance.
*/
class MKMapper : public InputMapper
{
public:
	MKMapper(Window* w, std::string filename);

	~MKMapper() {}

	//Fill and clear buffer
	void FillInputs();
	void ClearInputs();

	Vector3 GetMovement();
	float GetRotation();
	bool Fired();

	float* GetInputs() 
	{
		return inputs;
	}

	//Read in the button mappings.
	virtual void ReadMapping(std::string filename);

private:
	float inputs[6];
	Window* window;

	//Movement buttons can be read in from file.
	KeyboardKeys NEG_MOVEMENT_X;
	KeyboardKeys POS_MOVEMENT_X;
	KeyboardKeys POS_MOVEMENT_Z;
	KeyboardKeys NEG_MOVEMENT_Z;
	MouseButtons FIRE_BUTTON;
};
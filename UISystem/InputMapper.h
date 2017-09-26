#pragma once

#include "../nclgl/Vector3.h"
#define _USE_MATH_DEFINES 

/*
  Abstract class for input mapping for players.
  Ensures inputs are retrieved for correct functions
  in the game.
*/
class InputMapper
{
public:
	InputMapper()
	{
		prevRotation = 0;
	}

	virtual ~InputMapper() {}

	virtual void FillInputs() = 0;
	virtual void ClearInputs() = 0;

	virtual Vector3 GetMovement() = 0;
	virtual float GetRotation() = 0;

	virtual bool Fired() = 0;
	virtual bool Reload() = 0;

	Vector3 rawRotation;
protected:
	float prevRotation = 0;
};


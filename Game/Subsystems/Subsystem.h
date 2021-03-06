#pragma once

#include "../Profiler/Timer/SubsystemTimer.h"

/*
  All subsystems of the engine should inherit from this.
  The timer can be started and stopped as appropriate
  for each update function.
*/
class Subsystem
{
public:

	Subsystem() {}
	virtual ~Subsystem() {}

	virtual void Update(const float& deltatime = 0) = 0;

	SubsystemTimer updateTimer;
};


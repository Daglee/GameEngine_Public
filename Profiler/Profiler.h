#pragma once

#include "MemoryWatcher.h"
#include "FramerateCounter.h"
#include "../Game/Subsystem.h"
#include "../ResourceManagment/Resource.h"
#include "../nclgl/Text.h"
#include <map>

class Window;
class DataBase;
class Renderer;

/*
  Handles the framerate counter, memory watcher
  and subsystem timers in one update function.
*/
class Profiler : public Subsystem, public Resource
{
public:
	Profiler(DataBase* db, Window* win, int numTimers);
	Profiler(DataBase* db);

	~Profiler() {}

	void Update(float deltatime = 0);

	//Name is used when displaying the information
	void AddSubSystemTimer(string name, SubsystemTimer* timer);

	void Read(string resourcename);
	void ReadParams(string params);

private:
	void UpdateProfiling();
	void RenderToScreen();

	void RenderMemory();
	void RenderFPSCounter();
	void RenderTimers();

	int	numTimers;
	int	numAdded = 0;
	bool renderingEnabled = false;

	Window*	window;
	DataBase* database;
	Renderer* renderer;
	MemoryWatcher memoryWatcher;
	FramerateCounter fpsCounter;
	map<string, SubsystemTimer*> timers;
};


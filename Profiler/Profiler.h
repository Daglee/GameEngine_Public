#pragma once

#include "MemoryWatcher.h"
#include "FramerateCounter.h"
#include "../Game/Subsystem.h"
#include "../ResourceManagment/Resource.h"
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

	void Update(const float& deltatime) override;

	//Name is used when displaying the information
	void AddSubSystemTimer(string name, SubsystemTimer* timer);

	void Read(const string resourcename) override;
	void ReadParams(const string params) override;

private:
	void UpdateProfiling();
	void RenderToScreen();

	void RenderMemory() const;
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


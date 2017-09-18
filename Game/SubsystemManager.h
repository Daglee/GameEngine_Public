#pragma once

#include "../nclgl/Renderer.h"
#include "Subsystem.h"
#include "../Threading/ThreadPool.h"

class DataBase;

/*
  Updates the subsystems of the game on
  each iteration of the game loop.
*/
class SubsystemManager
{
public:
	SubsystemManager(DataBase* database);

	~SubsystemManager() {}

	//Iterates the game loop
	void ThreadedUpdate(float deltatime);
	void Update(float deltatime);

private:
	Renderer* renderer;
	ThreadPool* threadPool;

	enum class SUBSYSTEM_INDEXES;
	Subsystem* subsystems[5];
	int updateCount = 0;
};


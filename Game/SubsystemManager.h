#pragma once

#include "../UISystem/InputManager.h"
#include "../Threading/ThreadManager.h"
#include "../nclgl/Renderer.h"
#include "../nclgl/Window.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../Physics/PhysicsEngine.h"
#include "../Profiler/Profiler.h"
#include "Subsystem.h"

#include "../Threading/ThreadPool.h"
#include <ctime>

/*
  Updates any subsystems passed to it on each
  iteration of the gameloop.
*/
class SubsystemManager
{
public:
	SubsystemManager(
		InputManager* inputManager, ThreadPool* threadPool,
		Renderer* renderer, Window* window, FSMManager* gamelogic, 
		PhysicsEngine* physicsEngine, Profiler* profiler);

	~SubsystemManager() {}

	//Iterates the game loop
	void ThreadedUpdate(float deltatime);
	void Update(float deltatime);

private:
	InputManager* inputManager;
	ThreadManager* threadManager;
	Renderer* renderer;
	Window* window;
	FSMManager* gamelogic;
	PhysicsEngine* physicsEngine;
	Profiler* profiler;

	ThreadPool* threadPool;

	Subsystem* subsystems[5];
};


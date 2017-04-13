#include "SubsystemManager.h"

SubsystemManager::SubsystemManager(
	InputManager* inputManager, ThreadPool* threadPool,
	Renderer* renderer, Window* window, FSMManager* gamelogic, 
	PhysicsEngine* physicsEngine, Profiler* profiler) 
{
	this->inputManager	= inputManager;
	this->renderer		= renderer;
	this->window		= window;
	this->gamelogic		= gamelogic;
	this->physicsEngine = physicsEngine;
	this->profiler		= profiler;
	this->threadPool	= threadPool;
}

//Game loop iteration!
void SubsystemManager::UpdateAll(float deltatime) 
{
	//OpenGL doesn't like to be threaded...
	renderer->Update(deltatime);

	//But these subsystems do...

	auto physicsupdate = threadPool->submitJob([](	//A promise that it will get done...
		PhysicsEngine* pe, float dt) {				//Any parameters needed...
		pe->Update(dt);								//The function to call...
	}, std::cref(physicsEngine), deltatime);		//What we are passing in...
	//Equivalent to "physicsEngine->Update(deltatime);".

	auto inputupdate = threadPool->submitJob([](
		InputManager* im, float dt) {
		im->Update(dt);
	}, std::cref(inputManager), deltatime);

	auto gamelogicupdate = threadPool->submitJob([](
		FSMManager* gl, float dt) {
		gl->Update(dt);
	}, std::cref(gamelogic), deltatime);

	auto profilerupdate = threadPool->submitJob([](
		Profiler* p, float dt) {
		p->Update(dt);
	}, std::cref(profiler), deltatime);

	auto audioupdate = threadPool->submitJob([](
		float dt) {
		AudioManager::GetInstance()->Update(dt);
	}, deltatime);
}
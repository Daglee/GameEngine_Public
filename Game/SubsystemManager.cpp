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
	/*
	  The pause button MUST be checked via the thread pool
	  itself as the UI System will be suspended and no more
	  updates will be performed on it.
	*/
	threadPool->CheckPause();

	//OpenGL doesn't like to be threaded...
	renderer->Update(deltatime);

	//But these subsystems do...

	auto physicsupdate = threadPool->SubmitJob([](	//A promise that it will get done...
		PhysicsEngine* pe, float dt) {				//Any parameters needed...
		pe->Update(dt);								//The function to call...
	}, std::cref(physicsEngine), deltatime);		//What we are passing in...
	//Equivalent to "physicsEngine->Update(deltatime);".

	auto inputupdate = threadPool->SubmitJob([](
		InputManager* im, float dt) {
		im->Update(dt);
	}, std::cref(inputManager), deltatime);

	auto gamelogicupdate = threadPool->SubmitJob([](
		FSMManager* gl, float dt) {
		gl->Update(dt);
	}, std::cref(gamelogic), deltatime);

	auto profilerupdate = threadPool->SubmitJob([](
		Profiler* p, float dt) {
		p->Update(dt);
	}, std::cref(profiler), deltatime);

	auto audioupdate = threadPool->SubmitJob([](
		float dt) {
		AudioManager::GetInstance()->Update(dt);
	}, deltatime);
}
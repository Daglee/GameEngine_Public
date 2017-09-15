#include "Profiler.h"

#include "../nclgl/Window.h"
#include "../ResourceManagment/DataBase.h"
#include "../nclgl/Renderer.h"
#include "../ResourceManagment/Log.h"

#define FRAME_MIN 1
#define TEXT_SIZE 15.0f

Profiler::Profiler(DataBase* db, Window* win, int numTimers) : ResourceBase()
{
	window			= win;
	database		= db;
	memoryWatcher	= MemoryWatcher(database->MaxSize(), database);
	fpsCounter		= FramerateCounter(window->GetTimer()->GetMS());
	renderer = database->GRenderer->Find("Renderer");

	//Upper bound of how many timers can be added
	this->numTimers = numTimers;

	this->SetResourceSize(sizeof(*this));
}

Profiler::Profiler(DataBase* db) : ResourceBase() 
{
	database = db;
	memoryWatcher = MemoryWatcher(database->MaxSize(), database);
	renderer = database->GRenderer->Find("Renderer");

	this->SetResourceSize(sizeof(*this));
}

void Profiler::Update(float deltatime)
{
	updateTimer.StartTimer();

	/*
	  Hard-coded to the "p" button...I don't know what the button in the 
	  top left of the keyboard, below ESC, is called...(The one that brings up 
	  the console to activate godmode on Fallout 3...).
	*/
	if (window->GetKeyboard()->KeyTriggered(KEYBOARD_P)) renderingEnabled = !renderingEnabled;

	UpdateProfiling();
	if (renderingEnabled)	RenderToScreen();
	else					updateTimer.StopTimer();

	//Timer is stopped in the render function if that is enabled.
	//So it can time itself, with a minimal loss of accuracy.
}

void Profiler::UpdateProfiling() 
{
	++fpsCounter.frames;

	memoryWatcher.Update();
	fpsCounter.CalculateFPS(window->GetTimer()->GetMS());
}

void Profiler::RenderToScreen()
{
	//MEMORY
	renderer->textbuffer.push_back(Text(
		("Used: " + std::to_string(memoryWatcher.percent) + "%"),
		Vector3(0, 30, 0), TEXT_SIZE));
	renderer->textbuffer.push_back(Text(
		("B Left: " + std::to_string(memoryWatcher.bytesleft)),
		Vector3(0, 50, 0), TEXT_SIZE));

	//FPS COUNTER
	fpsCounter.CalculateFPS(window->GetTimer()->GetMS());
	renderer->textbuffer.push_back(Text(
		("FPS: " + std::to_string(fpsCounter.fps)),
		Vector3(0, 0, 0), TEXT_SIZE));

	//TIMERS
	float offset = 100.0f;
	for each(std::pair<string, SubsystemTimer*> timer in timers) {
		renderer->textbuffer.push_back(Text(
			(timer.first + ":" + std::to_string(timer.second->timePassed)),
			Vector3(0, offset, 0), TEXT_SIZE));
		offset += 20.0f;
	}

	/*
	  Very slight loss in accuracy of the Profiler's own timer.
	  Couldnt think of another way to display a timer without
	  actually stopping the timer...
	*/
	updateTimer.StopTimer();
	renderer->textbuffer.push_back(Text(
		("Profiler:" + std::to_string(updateTimer.timePassed)),
		Vector3(0, offset, 0), TEXT_SIZE));
}

void Profiler::Read(string resourcename)
{
	this->SetResourceName(resourcename);
}

void Profiler::ReadParams(string params)
{
	std::istringstream iss(params);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };

	string name = tokens.at(0);
	string windowname = tokens.at(1);
	int num = atoi(tokens.at(2).c_str());

	numTimers = num;
	window = database->GWindow->Find(windowname);
	fpsCounter = FramerateCounter(window->GetTimer()->GetMS());

	this->SetResourceSize(sizeof(*this));
	this->SetResourceName(name);
}
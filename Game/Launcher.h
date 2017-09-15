#pragma once
#pragma comment(lib, "nclgl.lib")

#include "../NCLGL/window.h"
#include "../UISystem/InputManager.h"
#include "../ResourceManagment/Log.h"
#include "Subsystem.h"
#include "Player.h"
#include "Level.h"

#include <fstream>
#include <vector>

class DataBase;

/*
  Organises assets in the database to form a game.
   - Safety checks on initialisation of window and renderer.
   - Startup and level loading.
*/
class Launcher
{
public:
	Launcher(string rendererName, string windowName, DataBase* db);
	~Launcher() {}

	void Launch(string directory);

	void InitProfilerTimers();

	Window* GetWindow() const
	{
		return window;
	}

	const bool Initialised() const
	{
		return initialised;
	}

private:
	bool GraphicsExists();
	bool IsGraphicsInitialised();
	bool IsSubSystemNull(Subsystem* subsystem, std::string name);
	void AttachGraphicsAndInput();

	InputManager* inputManager;
	DataBase*	database;
	Renderer*	renderer;
	Window*		window;

	string		windowName;
	string		rendererName;

	bool initialised = false;
};


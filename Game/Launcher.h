#pragma once
#pragma comment(lib, "nclgl.lib")

#include "../NCLGL/window.h"
#include "../UISystem/InputManager.h"
#include "Subsystem.h"
#include "Player.h"

class DataBase;

/*
  Organises assets in the database to form a game.
   - Safety checks on initialisation of window and renderer.
   - Startup and level loading.
*/
class Launcher
{
public:
	Launcher(const string rendererName, const string windowName, DataBase* db);
	~Launcher() {}

	void Launch(const string directory);

	void InitProfilerTimers() const;

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
	bool IsSubSystemNull(Subsystem* subsystem, const string name);
	void AttachGraphicsAndInput();

	InputManager* inputManager;
	DataBase* database;
	Renderer* renderer;
	Window*	 window;

	string windowName;
	string rendererName;

	bool initialised = false;
};


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
class Game
{
public:
	//Initialise with pointers to what is in the database
	Game(Renderer* rend, Window* wind, DataBase* db);

	//Initialise by searching the database
	Game(string rendererName, string windowName, DataBase* db);
	~Game() {}

	void InitialisePlayers();

	void StartUp(string directory);
	void LoadLevel(string level);
	void ExitLevel();

	Renderer* GetRenderer() const
	{
		return renderer;
	}

	Window* GetWindow() const
	{
		return window;
	}

	InputManager* GetInputManager()
	{
		return inputManager;
	}

	bool Initialised() const
	{
		return initialised;
	}

private:
	InputManager* inputManager;
	DataBase*	database;
	Renderer*	renderer;
	Window*		window;

	string		windowName;
	string		rendererName;

	Level*		currentLevel;

	bool initialised = false;
};


#pragma comment(lib, "nclgl.lib")
#pragma comment(lib, "Physics.lib")
#pragma comment(lib, "Threading.lib")
#pragma comment(lib, "GameLogicFSM.lib")
#pragma comment(lib, "Profiler.lib")
#pragma comment(lib, "UISystem.lib")
#pragma comment(lib, "ResourceManagment.lib")

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Launcher.h"
#include "SubsystemManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../GameLogicFSM/MessageSystem.h"
#include "LevelManager.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Renderer.h"

#include "../ResourceManagment/TableCreation.h"

/*  
	Important Notes:
	Up to 5 players supported. Player 1 will use mouse and keyboard.
	Xbox One controllers are supported and the official wireless/bluetooth
	adapter is compatible.

	Controls: (There is also an in-game tutorial)
	Mouse + Keyboard: WASD = Move, Mouse = Aim, Left Click = shoot.
	Controller: Left Stick = Move, Right Stick = Aim, Right Trigger = shoot.

	Press "P" on the keyboard to toggle the profiler being displayed on screen.
	
	Press "Escape" to toggle whether the game is paused or not. The profiler 
	will not be displayed when paused.
	
	Press "L" to stop the game.

	===========================================================================
	 .-.			 .-.			  .--.
	| OO|			| OO|			 / _.-' .-.   .-.  .-.   .''.
	|   |			|   |			 \  '-. '-'   '-'  '-'   '..'
	'^^^'			'^^^'			  '--'
	===========================================================================
*/


int main()
{
	AudioManager::Initialise();
	MessageSystem::Initialise();

	DataBase* database = new DataBase();
	Launcher* game = new Launcher("Renderer", "Window", database);

	TableCreation tables(database);
	tables.AddTablesToDatabase();

	database->ReserveMemoryForAllTables();

	game->Launch("../Data/startup.txt");

	Camera*	camera	= static_cast<Camera*>(database->GetTable("GCamera")->GetResources()->Find("Camera"));
	Window*	window	= static_cast<Window*>(database->GetTable("GWindow")->GetResources()->Find("Window"));
	Renderer* renderer = static_cast<Renderer*>(database->GetTable("GRenderer")->GetResources()->Find("Renderer"));

	renderer->SetCamera(camera);
	AudioManager::GetInstance()->SetListener(camera->GetSceneNode());

	game->InitProfilerTimers();

	SubsystemManager subsystems(database);

	LevelManager* lvlManager = new LevelManager(database, "../Data/Levels/LevelList.txt");
	lvlManager->LoadFirstLevel();

	//Game loop...
	while (window->UpdateWindow() && window->running)
	{
		float deltatime = window->GetTimer()->GetTimedMS();

		lvlManager->Update(deltatime);
		subsystems.Update(deltatime);
	}

	//Game logic handles exiting the level.
	delete game;
	delete lvlManager;
	delete database;

	_CrtDumpMemoryLeaks();

    return 0;
}
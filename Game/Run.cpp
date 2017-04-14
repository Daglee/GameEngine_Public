#pragma comment(lib, "nclgl.lib")
#pragma comment(lib, "Physics.lib")
#pragma comment(lib, "Threading.lib")
#pragma comment(lib, "GameLogicFSM.lib")
#pragma comment(lib, "Profiler.lib")
#pragma comment(lib, "UISystem.lib")

#include "Game.h"
#include "SubsystemManager.h"
#include "../ResourceManagment/Log.h"
#include "../ResourceManagment/DataBase.h"
#include "../Threading/ThreadManager.h"
#include "../NCLGL/window.h"
#include "../NCLGL/Renderer.h"
#include "../Physics/PhysicsEngine.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../Profiler/Profiler.h"

/*
	Important Notes:
	Up to 5 players supported. Player 1 will use mouse and keyboard.
	Xbox One controllers are supported and the official wireless/bluetooth
	adapter IS compatible.

	Controls::
	Mouse + Keyboard: WASD = Move, Mouse = Aim, Left Click = shoot.
	Controller: Left Stick = Move, Right Stick = Aim, Right Trigger = shoot.

	Press "P" on the keyboard to toggle the profiler being displayed on screen.
	Press "Escape" to toggle whether the game is paused or not. The profiler 
	will not be displayed when paused.

	===========================================================================
			   .-.			 .-.			  .--.                         
			  | OO|			| OO|			 / _.-' .-.   .-.  .-.   .''.  
			  |   |			|   |			 \  '-. '-'   '-'  '-'   '..'  
			  '^^^'			'^^^'			  '--'                         
	===========================================================================
*/
int main()
{
	//Initialise the singleton (Gets added to database next).
	AudioManager::Initialise();

	//Gets initialised on start up.
	DataBase* database = new DataBase();

	//Hook a few things up...
	Game* game = new Game("Renderer", "Window", database);

	//Base assets such as the renderer, physics engine etc get loaded in.
	game->StartUp("../Data/startup.txt");

	//More specific assets for a level such as a set number of spheres get loaded in.
	game->LoadLevel("../Data/Levels/Directories/Arena1.txt");

	if (!game->Initialised()) return -1;

	game->GetWindow()->LockMouseToWindow(true);
	game->GetWindow()->ShowOSPointer(false);

	//Returns the pointers now and send the to the appropriate subsystems.
	ThreadPool*		tPool		 = database->GThreadPool->Find("ThreadPool");
	Renderer*		renderer	 = database->GRenderer->Find("Renderer");
	Window*			win			 = database->GWindow->Find("Window");
	PhysicsEngine*	physics		 = database->GPhysicsEngine->Find("PhysicsEngine");
	FSMManager*		gamelogic	 = database->GFSMManager->Find("GFSMManager");
	Profiler*		profiler	 = database->GProfiler->Find("Profiler");
	InputManager*	inputManager = database->GInputManager->Find("InputManager");
	Camera*			camera		 = database->GCamera->Find("Camera");

	physics->gamelogic = gamelogic;

	//Set up any timers we want displayed on screen...
	renderer->SetCamera(camera);
	profiler->AddSubSystemTimer("Renderer", &renderer->updateTimer);
	profiler->AddSubSystemTimer("PhysicsE", &physics->updateTimer);
	profiler->AddSubSystemTimer("Gamelogic", &gamelogic->updateTimer);
	profiler->AddSubSystemTimer("Input", &inputManager->updateTimer);
	profiler->AddSubSystemTimer("Audio", &AudioManager::GetInstance()->updateTimer);

	AudioManager::GetInstance()->SetListener(camera->GetSceneNode());

	//Which subsystems will be updated...
	SubsystemManager subsystems(inputManager, tPool, renderer, win,
		gamelogic, physics, profiler);

	//Game loop...
	while (game->GetWindow()->UpdateWindow() && game->GetWindow()->running == 1) {
		subsystems.UpdateAll(win->GetTimer()->GetTimedMS());
	}

	//I'm just loading in a level and releasing it on shutdown. 
	//In reality, this function could be called at any time.
	game->ExitLevel();

	//Free up all the memory...
	delete database;

	//Done. GG.
    return 0;
}
#pragma comment(lib, "nclgl.lib")
#pragma comment(lib, "Physics.lib")
#pragma comment(lib, "Threading.lib")
#pragma comment(lib, "GameLogicFSM.lib")
#pragma comment(lib, "Profiler.lib")
#pragma comment(lib, "UISystem.lib")
#pragma comment(lib, "ResourceManagment.lib")

#include "Game.h"
#include "SubsystemManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../GameLogicFSM/MessageSystem.h"

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
	//Initialise the singleton (Gets added to database next).
	AudioManager::Initialise();
	MessageSystem::Initialise();

	//Gets initialised on start up.
	DataBase* database = new DataBase();

	//Prepare for start up and initialise database.
	Game* game = new Game("Renderer", "Window", database);

	//Base assets such as the renderer, physics engine etc get loaded in.
	game->StartUp("../Data/startup.txt");

	//More specific assets for a level such as a set number of spheres get loaded in.
	game->LoadLevel("../Data/Levels/Directories/Arena1.txt");

	if (!game->Initialised()) return -1;

	game->GetWindow()->LockMouseToWindow(true);
	game->GetWindow()->ShowOSPointer(false);

	//Return the pointers now and send the to the appropriate subsystems.
	Camera*	camera	= database->GCamera->Find("Camera");
	Window*	win		= database->GWindow->Find("Window");

	database->GRenderer->Find("Renderer")->SetCamera(camera);
	AudioManager::GetInstance()->SetListener(camera->GetSceneNode());

	//Set up any timers we want displayed on screen...
	game->InitProfilerTimers();

	//Prepare all subsystems for updating...
	SubsystemManager subsystems(database);

	//Game loop...
	while (game->GetWindow()->UpdateWindow() && game->GetWindow()->running) {
		subsystems.ThreadedUpdate(win->GetTimer()->GetTimedMS());
	}

	//I'm just loading in a level and releasing it on shutdown. 
	//In reality, this function could be called at any time.
	game->ExitLevel();

	//Free up all the memory...
	delete database;

	//Done. GG.
    return 0;
}
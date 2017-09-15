#pragma comment(lib, "nclgl.lib")
#pragma comment(lib, "Physics.lib")
#pragma comment(lib, "Threading.lib")
#pragma comment(lib, "GameLogicFSM.lib")
#pragma comment(lib, "Profiler.lib")
#pragma comment(lib, "UISystem.lib")
#pragma comment(lib, "ResourceManagment.lib")

#include "Launcher.h"
#include "SubsystemManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../GameLogicFSM/MessageSystem.h"
#include "LevelManager.h"

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

	database->InitialiseDataBase();
	game->Launch("../Data/startup.txt");

	if (!game->Initialised()) return -1;

	Camera*	camera	= database->GCamera->Find("Camera");
	Window*	win		= database->GWindow->Find("Window");
	database->GRenderer->Find("Renderer")->SetCamera(camera);
	AudioManager::GetInstance()->SetListener(camera->GetSceneNode());

	//Set up any timers we want displayed on screen...
	game->InitProfilerTimers();

	SubsystemManager subsystems(database);

	LevelManager* lvlManager = new LevelManager(database, "../Data/Levels/LevelList.txt");
	lvlManager->LoadFirstLevel();

	//Game loop...
	while (game->GetWindow()->UpdateWindow() && game->GetWindow()->running) {
		float deltatime = win->GetTimer()->GetTimedMS();

		lvlManager->Update(deltatime);
		subsystems.Update(deltatime);
	}

	//Game logic handles exiting the level.

	delete database;

    return 0;
}
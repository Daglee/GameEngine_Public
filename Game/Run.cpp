#pragma comment(lib, "nclgl.lib")
#pragma comment(lib, "Physics.lib")
#pragma comment(lib, "Threading.lib")
#pragma comment(lib, "GameLogicFSM.lib")
#pragma comment(lib, "Profiler.lib")
#pragma comment(lib, "UISystem.lib")
#pragma comment(lib, "ResourceManagement.lib")

#include "Structure/Launcher.h"
#include "Subsystems/SubsystemManager.h"
#include "../ResourceManagement/Database/DataBase.h"
#include "../GameLogicFSM/Messaging/MessageSystem.h"
#include "Levels/Level Configuration/LevelManager.h"
#include "../nclgl/Rendering/View/Camera.h"
#include "../nclgl/Rendering/Renderer.h"

#include "../ResourceManagement/Database/TableCreation.h"

/*  
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

    return 0;
}
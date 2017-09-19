#include "InputManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../Threading/ThreadPool.h"
#include "../Game/ScoreBoard.h"
#include "../Game/Pistol.h"
#include "../Game/RocketLauncher.h"
#include "../Game/TopDownController.h"
#include "../Game/SpawnSystem.h"
#include "PlayerInputConnection.h"

const int MAX_CONTROLLERS = 3;

InputManager::InputManager(ThreadPool* t)
{
	gamepads = new Gamepad*[6];
	players = new Player*[6];

	threadPool = t;

	SpawnSystem::Initialise();
}

InputManager::~InputManager()
{
	delete gamepads;
	delete players;
	delete playerConfig;
}

/*
  vector of players and gamepads point to all
  memory addresses of players and gamepads
  that have been loaded into the database
*/
void InputManager::ConnectToDataBase(DataBase* databaseToConnect)
{
	this->database = databaseToConnect;

	//Retrieve everything from the database
	for (int i = 0; i < 6; i++)
	{
		string id = std::to_string(i);

		string name = "gamepad" + id;
		gamepads[i] = database->Gamepads->Find(name);;

		name = "player" + id;
		players[i] = database->Players->Find(name);
	}

	/*
	  Store these pointers so we dont have to retrieve
	  them again every time a player is added.
	*/
	StoreDatabase();

	//There must always be a player 1 (M&K)
	connectedPlayers.push_back(players[0]);

	PlayerInputConnection connection(players[0]);
	connection.ConnectPlayerToKeyboard(window, threadPool);

	IngamePlayer ingamePlayer(systems);
	ingamePlayer.AddPlayerToGame(players[0]);

	playerConfig->InitialisePlayer(players[0], connectedPlayers.size());
}

void InputManager::StoreDatabase()
{
	BasePlayerSystems components;
	components.defaultBulletMesh = database->OBJMeshes->Find("../Data/Meshes/sphere.obj");
	components.defaultPlayerMesh = database->OBJMeshes->Find("../Data/Meshes/cube.obj");

	systems.renderer = database->GRenderer->Find("Renderer");
	systems.physicsEngine = database->GPhysicsEngine->Find("PhysicsEngine");
	systems.window = database->GWindow->Find("Window");
	systems.threadPool = threadPool;
	systems.database = database;

	this->window = systems.window;

	playerConfig = new PlayerConfiguration(components);
}

/*
  Will retrieve any connected gamepads
  If this gamepad was not previously connected,
  a new player is initialised
*/
std::vector<Gamepad*>* InputManager::ConnectGamepads(bool reconnection)
{
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (gamepads[i]->Connected())
		{
			//Check if the gamepad is already connected and inside the vector
			bool gamepadNotStored = std::find(connectedGPads.begin(), connectedGPads.end(), gamepads[i]) == connectedGPads.end();
			if (gamepadNotStored)
			{
				StoreGamepadPlayer(i);

				if (!reconnection)
				{
					Player* playerToConnect = players[i + 1];

					PlayerInputConnection connection(playerToConnect);
					connection.ConnectPlayerToController(gamepads[i]);

					IngamePlayer ingamePlayer(systems);
					ingamePlayer.AddPlayerToGame(playerToConnect);

					playerConfig->InitialisePlayer(playerToConnect, connectedPlayers.size());
				}
			}

		}
	}

	return &connectedGPads;
}

/*
  Check that anything in the gamepads vector is still connected.
  Erase if that connection has been broken
*/
void InputManager::UpdateConnections()
{
	vector<Gamepad*>::iterator iterator = connectedGPads.begin();

	while (iterator != connectedGPads.end())
	{
		if (!(*iterator)->Connected())
		{
			iterator = connectedGPads.erase(iterator);
		}
		else
		{
			++iterator;
		}
	}
}

void InputManager::ReInitialisePlayers()
{
	MKMapper* mkm = new MKMapper(window, "../Data/ButtonMapping/MKMap.txt");
	players[0]->GetPlayerController()->SetInputMapper(mkm);
	connectedPlayers.push_back(players[0]);
	threadPool->pauseButton = mkm->PAUSE;

	ScoreBoard::GetInstance()->AddEntry(new Entry(players[0]->GetRigidBody()->tag, 0));

	ConnectGamepads(true);
	for (int i = 1; i < connectedPlayers.size(); ++i)
	{
		ScoreBoard::GetInstance()->AddEntry(new Entry("player" + std::to_string(i + 1), 0));
	}
}

void InputManager::StoreGamepadPlayer(int index)
{
	connectedGPads.push_back(gamepads[index]);
	connectedPlayers.push_back(players[index + 1]);
}

//Fill the buffers and use them!
void InputManager::Update(float deltatime)
{
	updateTimer.StartTimer();

	for (vector<Player*>::iterator player = connectedPlayers.begin();
		player != connectedPlayers.end(); player++)
	{
		(*player)->Update(deltatime, window->GetTimer()->GetMS());
		(*player)->GetPlayerController()->GetInputMapper()->ClearInputs();
		(*player)->GetPlayerController()->GetInputMapper()->FillInputs();
		(*player)->ApplyInputs();
	}

	updateTimer.StopTimer();
}

void InputManager::ClearAll()
{
	connectedPlayers.clear();
	connectedGPads.clear();
}

void InputManager::Read(string resourcename)
{
	this->SetResourceName(resourcename);
}

void InputManager::ReadParams(string params)
{
	Read(params);
}
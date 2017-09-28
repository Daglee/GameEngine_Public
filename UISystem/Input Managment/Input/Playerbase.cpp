#include "Playerbase.h"

#include "../Player Configuration/PlayerInputConnection.h"
#include "../../MouseAndKeyboard/MKMapper.h"
#include "../ResourceManagement/Database/DataBase.h"
#include "../Game/Scoring/ScoreBoard.h"
#include "../Game/Players/Player.h"
#include "../Game/Players/Control/PlayerController.h"
#include "../nclgl/InputDevices/Gamepad.h"
#include "../nclgl/Rendering/Renderer.h"
#include "../Physics/Engine/PhysicsEngine.h"
#include "../Threading/ThreadPool/ThreadPool.h"

#include <string>

const int MAX_CONTROLLERS = 3;

Playerbase::Playerbase(DataBase* database)
{
	gamepads = new Gamepad*[6];
	players = new Player*[6];

	this->database = database;
	playerConfig = nullptr;
}

Playerbase::~Playerbase()
{
	delete gamepads;
	delete players;
	delete playerConfig;
}

void Playerbase::RetrieveGamepadsAndPlayers(DataBase* db)
{
	database = db;

	for (int i = 0; i < 6; i++)
	{
		const string id = to_string(i);

		const string gamepad = "gamepad" + id;
		gamepads[i] = static_cast<Gamepad*>(database->GetTable("Gamepads")->GetResources()->Find(gamepad));
		
		const string player = "player" + id;
		players[i] = static_cast<Player*>(database->GetTable("Players")->GetResources()->Find(player));
	}

	StoreDatabase();

	StorePlayer(0);
	PlayerInputConnection connection(players[0]);
	connection.ConnectPlayerToKeyboard(systems.window, systems.threadPool);

	AddPlayerToGame(players[0]);
}

void Playerbase::StoreDatabase()
{
	systems.renderer = static_cast<Renderer*>(database->GetTable("GRenderer")->GetResources()->Find("Renderer"));
	systems.physicsEngine = static_cast<PhysicsEngine*>(database->GetTable("PhysicsEngine")->GetResources()->Find("PhysicsEngine"));
	systems.window = static_cast<Window*>(database->GetTable("GWindow")->GetResources()->Find("Window"));
	systems.threadPool = static_cast<ThreadPool*>(database->GetTable("GThreadPool")->GetResources()->Find("ThreadPool"));
	systems.database = database;

	BasePlayerSystems components;
	components.defaultBulletMesh = static_cast<OBJMesh*>(database->GetTable("OBJMeshes")->GetResources()->Find("../Data/Meshes/sphere.obj"));
	components.defaultPlayerMesh = static_cast<OBJMesh*>(database->GetTable("OBJMeshes")->GetResources()->Find("../Data/Meshes/cube.obj"));

	playerConfig = new PlayerConfiguration(components);
}

void Playerbase::AddPlayerToGame(Player* player) const
{
	IngamePlayer ingamePlayer(systems);
	ingamePlayer.AddPlayerToGame(player);

	playerConfig->InitialisePlayer(player, connectedPlayers.size());
}

void Playerbase::ConnectGamepads(const bool reconnection)
{
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (gamepads[i]->Connected())
		{
			//Check if the gamepad is already connected and inside the vector
			const bool gamepadNotStored = find(connectedGPads.begin(), connectedGPads.end(), 
				gamepads[i]) == connectedGPads.end();
			
			if (gamepadNotStored)
			{
				StorePlayer(i + 1);
				StoreGamepad(i);

				if (!reconnection)
				{
					PlayerInputConnection connection(players[i + 1]);
					connection.ConnectPlayerToController(gamepads[i]);

					AddPlayerToGame(players[i + 1]);
				}
			}

		}
	}
}

void Playerbase::UpdateConnections()
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

void Playerbase::ReInitialisePlayers()
{
	MKMapper* mkm = new MKMapper(systems.window, "../Data/ButtonMapping/MKMap.txt");
	players[0]->GetPlayerController()->SetInputMapper(mkm);
	connectedPlayers.push_back(players[0]);
	systems.threadPool->pauseButton = mkm->PAUSE;

	ScoreBoard::GetInstance()->AddEntry(new Entry(players[0]->GetRigidBody()->tag, 0));

	ConnectGamepads(true);
	for (unsigned i = 1; i < connectedPlayers.size(); ++i)
	{
		ScoreBoard::GetInstance()->AddEntry(new Entry("player" + std::to_string(i + 1), 0));
	}
}

void Playerbase::StorePlayer(const int index)
{
	connectedPlayers.push_back(players[index]);
}

void Playerbase::StoreGamepad(const int index)
{
	connectedGPads.push_back(gamepads[index]);
}

void Playerbase::ClearAll()
{
	connectedPlayers.clear();
	connectedGPads.clear();
}
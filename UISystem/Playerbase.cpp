#include "Playerbase.h"

#include "../ResourceManagment/DataBase.h"
#include "PlayerInputConnection.h"
#include "../Game/ScoreBoard.h"

#include <string>

const int MAX_CONTROLLERS = 3;

Playerbase::Playerbase(DataBase* database)
{
	gamepads = new Gamepad*[6];
	players = new Player*[6];

	this->database = database;
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
		std::string id = std::to_string(i);

		std::string gamepad = "gamepad" + id;
		gamepads[i] = database->Gamepads->Find(gamepad);

		std::string player = "player" + id;
		players[i] = database->Players->Find(player);
	}

	StoreDatabase();

	StorePlayer(0);
	PlayerInputConnection connection(players[0]);
	connection.ConnectPlayerToKeyboard(systems.window, systems.threadPool);

	AddPlayerToGame(players[0]);
}

void Playerbase::StoreDatabase()
{
	systems.renderer = database->GRenderer->Find("Renderer");
	systems.physicsEngine = database->GPhysicsEngine->Find("PhysicsEngine");
	systems.window = database->GWindow->Find("Window");
	systems.threadPool = database->GThreadPool->Find("ThreadPool");
	systems.database = database;

	BasePlayerSystems components;
	components.defaultBulletMesh = database->OBJMeshes->Find("../Data/Meshes/sphere.obj");
	components.defaultPlayerMesh = database->OBJMeshes->Find("../Data/Meshes/cube.obj");

	playerConfig = new PlayerConfiguration(components);
}

void Playerbase::AddPlayerToGame(Player* player)
{
	IngamePlayer ingamePlayer(systems);
	ingamePlayer.AddPlayerToGame(player);

	playerConfig->InitialisePlayer(player, connectedPlayers.size());
}

void Playerbase::ConnectGamepads(bool reconnection)
{
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (gamepads[i]->Connected())
		{
			//Check if the gamepad is already connected and inside the vector
			bool gamepadNotStored = std::find(connectedGPads.begin(), connectedGPads.end(), gamepads[i]) == connectedGPads.end();
			
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
	for (int i = 1; i < connectedPlayers.size(); ++i)
	{
		ScoreBoard::GetInstance()->AddEntry(new Entry("player" + std::to_string(i + 1), 0));
	}
}

void Playerbase::StorePlayer(int index)
{
	connectedPlayers.push_back(players[index]);
}

void Playerbase::StoreGamepad(int index)
{
	connectedGPads.push_back(gamepads[index]);
}

void Playerbase::ClearAll()
{
	connectedPlayers.clear();
	connectedGPads.clear();
}
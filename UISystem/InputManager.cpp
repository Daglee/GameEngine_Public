#include "InputManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../Threading/ThreadPool.h"
#include "../Game/ScoreBoard.h"
#include "../Game/Pistol.h"
#include "../Game/RocketLauncher.h"
#include "../Game/TopDownController.h"

InputManager::InputManager(ThreadPool* t)
{
	gpad	= new Gamepad*[6];
	players	= new Player*[6];

	threadPool = t;
}

InputManager::~InputManager()
{
	delete gpad;
	delete players;
}

/*
  vector of players and gamepads point to all 
  memory addresses of players and gamepads 
  that have been loaded into the database
*/
void InputManager::ConnectToDataBase(DataBase* db)
{
	database = db;

	//Retrieve everything from the database
	for (int i = 0; i < 6; i++) {
		string id = std::to_string(i);

		string name = "gamepad" + id;
		gpad[i] = database->Gamepads->Find(name);;

		name = "player" + id;
		players[i] = database->Players->Find(name);
	}

	/*
	  Store these pointers so we dont have to retrieve 
	  them again every time a player is added.
	*/
	defaultBulletMesh	= database->OBJMeshes->Find("../Data/Meshes/sphere.obj");
	defaultPlayerMesh	= database->OBJMeshes->Find("../Data/Meshes/cube.obj");
	renderer			= database->GRenderer->Find("Renderer");
	physicsEngine		= database->GPhysicsEngine->Find("PhysicsEngine");
	window				= database->GWindow->Find("Window");

	//There must always be a player 1 (M&K)
	connectedPlayers.push_back(players[0]);
	InitialisePlayer(players[0]);
}

/*
  Will retrieve any connected gamepads
  If this gamepad was not previously connected,
  a new player is initialised
*/
std::vector<Gamepad*>* InputManager::ConnectedGamePads(bool reconnection)
{
	for (int i = 0; i < 3; i++) {
		if (gpad[i]->Connected()) {

			//Check if the gamepad is already connected and inside the vector
			if (std::find(connectedGPads.begin(), connectedGPads.end(), gpad[i]) == connectedGPads.end()) {
				//Not found, therefore add them
				connectedGPads.push_back(gpad[i]);
				connectedPlayers.push_back(players[i + 1]);
				if(!reconnection) InitialisePlayer(players[i + 1], gpad[i]);
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
	vector<Gamepad*>::iterator i = connectedGPads.begin();
	while (i != connectedGPads.end()) {
		if (!(*i)->Connected()) {
			i = connectedGPads.erase(i);
		}
		else ++i;
	}
}

void InputManager::ReInitialisePlayers()
{
	MKMapper* mkm = new MKMapper(window, "../Data/ButtonMapping/MKMap.txt");
	players[0]->GetPlayerController()->SetInputMapper(mkm);
	connectedPlayers.push_back(players[0]);
	threadPool->pauseButton = mkm->PAUSE;

	ScoreBoard::GetInstance()->AddEntry(new Entry(players[0]->GetRigidBody()->tag, 0));

	ConnectedGamePads(true);
	for (int i = 1; i < connectedPlayers.size(); ++i) {
		ScoreBoard::GetInstance()->AddEntry(new Entry("player" + std::to_string(i + 1), 0));
	}
}

/*
  Initialise players with default settings.
  Attach a gamepad to each player and send to appropraite renderer and physics engine.
  Player2 = Gamepad2 etc
*/
void InputManager::InitialisePlayer(Player* p, Gamepad* gp) 
{
	GamepadMapper* gm = new GamepadMapper();
	gm->SetGamePad(gp);
	p->SetPlayerController(new TopDownController(gm));
	p->GetPlayerController()->SetInputMapper(gm);
	p->gun = new Pistol(database, renderer, physicsEngine, defaultBulletMesh);
	SetPlayerParameters(p);
}

/*
  M&K player has different inputs, therefore different parameters required 
  for gameplay mechanics such as Gun.
*/
void InputManager::InitialisePlayer(Player* p)
{
	MKMapper* mkm = new MKMapper(window, "../Data/ButtonMapping/MKMap.txt");
	p->SetPlayerController(new TopDownController(mkm));
	p->GetPlayerController()->SetInputMapper(mkm);

	threadPool->pauseButton = mkm->PAUSE;

	p->gun = new Pistol(database, renderer, physicsEngine, defaultBulletMesh);
	SetPlayerParameters(p);
}

void InputManager::SetPlayerParameters(Player* p)
{
	p->GetPlayerModel()->UpdateMesh(defaultPlayerMesh);
	p->UpdatePhysics(physicsEngine);
	p->UpdateRenderer(renderer);

	Vector3 spawnPos(-1 * (float)(50 * connectedPlayers.size()), 100, 1 * (float)(50 * connectedPlayers.size()));
	p->AddSpawnPoint(spawnPos);

	p->GetRigidBody()->UpdatePosition(spawnPos);

	p->teamID = connectedPlayers.size(); //Just for now (free for all)...
	string tag = "player" + std::to_string(connectedPlayers.size());
	p->GetRigidBody()->tag = tag;
	p->GetPlayerModel()->playertag = tag;
	p->walkingSoundName = "walkingsound" + tag;
	p->gun->parent = tag;
	p->playerModelMesh = defaultPlayerMesh;

	p->GetPlayerController()->SetCharacterModel(p->GetPlayerModel());
	p->GetPlayerController()->SetRigidBody(p->GetRigidBody());
	p->GetPlayerController()->SetMovementSound(p->walkingSoundName);

	p->gunInput = new GunInput(p->GetPlayerController()->GetInputMapper(), p->gun,
		WeaponData(&p->GetRigidBody()->lastPosition, p->GetIDNumber()));
	p->PrepareHUD();

	SoundNode* walkingSound = new SoundNode(
		new Sound("../Data/Sounds/41579__erdie__steps-on-stone01.wav"),
		p->GetPlayerModel());

	AudioManager::GetInstance()->AddSound("walkingsound" + tag, walkingSound);

	ScoreBoard::GetInstance()->AddEntry(new Entry(tag, 0));
}

//Fill the buffers and use them!
void InputManager::Update(float deltatime)
{
	updateTimer.StartTimer();

	for (vector<Player*>::iterator i = connectedPlayers.begin();
		i != connectedPlayers.end(); i++) {
		(*i)->Update(deltatime, window->GetTimer()->GetMS());
		(*i)->GetPlayerController()->GetInputMapper()->ClearInputs();
		(*i)->GetPlayerController()->GetInputMapper()->FillInputs();
		(*i)->ApplyInputs();
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
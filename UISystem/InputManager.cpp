#include "InputManager.h"
#include "../ResourceManagment/DataBase.h"

InputManager::InputManager()
{
	gpad	= new Gamepad*[6];
	players	= new Player*[6];
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
	defaultBulletMesh	= database->OBJMeshes->Find("../../Meshes/sphere.obj");
	defaultPlayerMesh	= database->OBJMeshes->Find("../../Meshes/cube.obj");
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
std::vector<Gamepad*>* InputManager::ConnectedGamePads() 
{
	for (int i = 0; i < 3; i++) {
		if (gpad[i]->Connected()) {

			//Check if the gamepad is already connected and inside the vector
			if (std::find(connectedGPads.begin(), connectedGPads.end(), gpad[i]) == connectedGPads.end()) {
				//Not found, therefore add them
				connectedGPads.push_back(gpad[i]);
				connectedPlayers.push_back(players[i + 1]);
				InitialisePlayer(players[i + 1], gpad[i]);
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

/*
  Initialise players with default settings.
  Attach a gamepad to each player and send to appropraite renderer and physics engine.
  Player2 = Gamepad2 etc
*/
void InputManager::InitialisePlayer(Player* p, Gamepad* gp) 
{
	FSM* f = new FSM(p->vars, "../../GameLogic/Player.txt");
	//f->BuildFSM();

	FSMManager* fm = database->GFSMManager->Find("GFSMManager");
	fm->AddFSM(f);

	GamepadMapper* gm = new GamepadMapper();
	gm->SetGamePad(gp);
	p->SetInputMapper(gm);
	float reloadSpeed = 2000;
	float bulletsPerMag = 20;
	float fireDelay = 150;
	p->gun = new Gun(database, renderer, physicsEngine, reloadSpeed,
		bulletsPerMag, 8, fireDelay, defaultBulletMesh);
	SetPlayerParameters(p);
}

/*
  M&K player has different inputs, therefore different parameters required 
  for gameplay mechanics such as Gun.
*/
void InputManager::InitialisePlayer(Player* p)
{
	FSM* f = new FSM(p->vars, "../../GameLogic/Player.txt");
	//f->BuildFSM();

	FSMManager* fm = database->GFSMManager->Find("GFSMManager");
	fm->AddFSM(f);

	MKMapper* mkm = new MKMapper(window, "../../ButtonMapping/MKMap.txt");
	p->SetInputMapper(mkm);
	float reloadSpeed = 2000;
	float bulletsPerMag = 20;
	float fireDelay = 150;
	p->gun = new Gun(database, renderer, physicsEngine, reloadSpeed,
		bulletsPerMag, 8, fireDelay, defaultBulletMesh);
	SetPlayerParameters(p);
}

void InputManager::SetPlayerParameters(Player* p)
{
	p->GetPlayerModel()->UpdateMesh(defaultPlayerMesh);
	p->UpdatePhysics(physicsEngine);
	p->UpdateRenderer(renderer);
	p->Move(Vector3(500, 100, 0 * connectedPlayers.size())); // REPLACE LATER
	p->GetRigidBody()->UpdatePosition(Vector3(0, 100, 50 * connectedPlayers.size()));
	p->teamID = connectedPlayers.size(); //Just for now (free for all)...

	string tag = "player" + std::to_string(connectedPlayers.size());
	p->GetRigidBody()->tag = tag;// p->GetResourceName();
	p->walkingSoundName = "walkingsound" + tag;

	SoundNode* walkingSound = new SoundNode(
		new Sound("../../Sounds/41579__erdie__steps-on-stone01.wav"),
		p->GetPlayerModel());

	AudioManager::GetInstance()->AddSound("walkingsound" + tag, walkingSound);
}

//Fill the buffers and use them!
void InputManager::Update(float deltatime)
{
	updateTimer.StartTimer();

	for (vector<Player*>::iterator i = connectedPlayers.begin();
		i != connectedPlayers.end(); i++) {
		(*i)->Update(deltatime, window->GetTimer()->GetMS());
		(*i)->GetInputMapper()->ClearInputs();
		(*i)->GetInputMapper()->FillInputs();
		(*i)->ApplyInputs();
	}

	updateTimer.StopTimer();
}

void InputManager::Read(string resourcename) 
{
	this->SetResourceName(resourcename);
}

void InputManager::ReadParams(string params) 
{
	Read(params);
}

#define NOMINMAX
#include "Player.h"
#define NOMINMAX

#include "../ResourceManagment/DataBase.h"
#include "../UISystem/InputMapper.h"
#include "../Physics/PhysicsEngine.h"
#include "../Physics/RigidBody.h"
#include "../../nclgl/Renderer.h"
#include "../GameLogicFSM/MessageSystem.h"
#include "ScoreBoard.h"

#include "RocketLauncher.h"
#include "Pistol.h"

#define PLAYER_MASS 500.0f
#define PLAYER_DRAG 0.7f
#define PLAYER_GRAVITY -0.1f

Player::Player(DataBase* database, int id) : ResourceBase(), FSMUnit("player" + to_string(id), database)
{
	//Load in a character model
	playerModel = new CharacterModel(rigidBody.tag, CHARCT_MODEL_LOC);
	
	//Just some default values in case game logic fails
	playerModel->SetTransform(Vector3(0, 0, 0)); 
	playerModel->UpdateColour(Vector4(1, 1, 0, 1));

	rigidBody = RigidBody(Vector3(), Vector3(), 
		PLAYER_MASS, PLAYER_DRAG, PLAYER_GRAVITY);
	rigidBody.isStatic = false;
	rigidBody.atRest = false;
	rigidBody.parentMesh = playerModel;

	SphereCollider* rb = new SphereCollider(playerModel->body.GetBoundingRadius()
	+ (playerModel->uleftArm.GetBoundingRadius() + playerModel->urightArm.GetBoundingRadius()));
	rigidBody.collider = rb;
	rigidBody.collider->position = rigidBody.lastPosition;

	lifeSpan = LifeSpan(100.0f, 120);

	//For the game logic FSM
	vars->insert({ "colourr", &colourr });
	vars->insert({ "colourg", &colourg });
	vars->insert({ "colourb", &colourb });
	vars->insert({ "coloura", &coloura });
	vars->insert({ "timer", &timer });
	vars->insert({ "collider", &collider });
	vars->insert({ "health", lifeSpan.GetCurrentHealth() });
	vars->insert({ "teamid", &teamid }); 
	vars->insert({ "killstreak", &killstreak });
	vars->insert({ "ypos", &rigidBody.lastPosition.y });

	idNumber = id;
	this->database = database;

	this->SetResourceSize(sizeof(*this));
}

Player::~Player()
{
	delete playerModel;
	delete controller;
	delete gunInput;
}


void Player::ApplyInputs()
{
	if (!lockInputs) {
		gunInput->ApplyInputs();
		controller->ApplyInputs();
	}
}

void Player::Move(const Vector3& pos) 
{
	rigidBody.UpdatePosition(pos);
}

void Player::Update(const float& msec, const float& timer) 
{
	this->timer += msec / 1000.0f;

	playerModel->UpdateColour(Vector4(colourr, colourg, colourb, coloura));
	playerModel->Update(msec);

	CheckHealth();
	AddPoints();
	CheckGunChange();
	DisplayHUD();
}

void Player::CheckHealth()
{
	if (lifeSpan.IsDead())
	{
		if (lifeSpan.HasNotBegunRespawn())
		{
			Despawn();
			lifeSpan.IncrementFramesSpentDead();
		}
		else if (lifeSpan.TimeToRespawn())
		{
			Respawn(spawnPoints[ChooseRandomSpawnPoint()]);
		}
		else
		{
			lifeSpan.IncrementFramesSpentDead();
		}
	}
}

void Player::AddPoints()
{
	//Add any points that are pending from the game logic.
	if (MessageSystem::GetInstance()->MessageTransmitting(Log::Hash(rigidBody.tag + "addkillstreakpoints"))) {
		ScoreBoard::GetInstance()->UpdateEntryScore(rigidBody.tag, killstreak);

		//Stop the event.
		MessageSystem::GetInstance()->StopTransmitting(Log::Hash(rigidBody.tag + "addkillstreakpoints")); 
	}
}

void Player::CheckGunChange()
{
	if (weaponChanger.RocketLauncherAvailable(rigidBody.tag))
	{
		weaponChanger.SetToRocketLauncher(gun);
		weaponChanger.AnnounceChange();
		weaponChanger.StopPreviousEvents();

		gunInput->SetWeapon(gun);
	}

	if (weaponChanger.MachineGunAvailable(rigidBody.tag))
	{
		weaponChanger.SetToMachineGun(gun);
		weaponChanger.AnnounceChange();
		weaponChanger.StopPreviousEvents();

		gunInput->SetWeapon(gun);
	}
}

void Player::DisplayHUD()
{
	string ammo = std::to_string(gun->bulletsPerMag - gun->bulletsFired) + "/" + std::to_string(gun->bulletsPerMag);
	Text hud(ammo, playerModel->GetTransform().GetPositionVector() + Vector3(0, 100, 0), 25.0f, true);
	renderer->textbuffer.push_back(hud);

	string teamchangetimer = std::to_string(5.0f - (std::max<float>)(0, ((std::min<float>)(this->timer, 5))));
	Text hudtimer(teamchangetimer.substr(0, 3), playerModel->GetTransform().GetPositionVector() + Vector3(0, 130, 0), 25.0f, true);
	renderer->textbuffer.push_back(hudtimer);

	if (gun->lastReloadTime + gun->reloadSpeed >= clock()) {
		string r = "reloading";
		Text reloading(r, playerModel->GetTransform().GetPositionVector() + Vector3(0, 160, 0), 25.0f, true);
		renderer->textbuffer.push_back(reloading);
	}

	Text name(rigidBody.tag, playerModel->GetTransform().GetPositionVector() + Vector3(30, 70, 0), 20.0f, true);
	renderer->textbuffer.push_back(name);
}

void Player::Despawn()
{
	MessageSystem::GetInstance()->Transmit(Log::Hash(rigidBody.tag + "dead"), false);

	CheckRagdollLimits(); //Too many ragdolls out?
	Ragdoll(); //Add a new one.

	//Move the player away from the screen and block inputs.
	rigidBody.UpdatePosition(rigidBody.lastPosition + Vector3(0, 100000, 0));
	rigidBody.gravity = 0;

	lockInputs = true;
}

void Player::Respawn(const Vector3& spawnPoint)
{
	lifeSpan.ResetHealth();
	lifeSpan.ResetFramesSpentDead();
	lockInputs = false;
	coloura = 1;
	rigidBody.gravity = PLAYER_GRAVITY;

	rigidBody.UpdatePosition(spawnPoint);
}

int Player::ChooseRandomSpawnPoint()
{
	std::random_device randomGenerator;
	std::mt19937 mersenneTwister(randomGenerator());
	std::uniform_int_distribution<int> range(0, spawnPoints.size() - 1);

	int randomSpawnPoint = range(mersenneTwister);

	return randomSpawnPoint;
}

void Player::CheckRagdollLimits()
{
	if (bodies.size() == playerModel->modelParts.size() * 2) {
		for each (PhysicsObject* obj in bodies)
		{
			renderer->RemoveSceneNode(obj->GetSceneNode());
			physicsEngine->RemoveRigidBody(obj->GetRigidBody());
			delete obj;
		}

		bodies.clear();
	}
}

void Player::Ragdoll()
{
	//Just copy the character model data into a set of physics objects.
	for each (SceneNode* modelpart in playerModel->modelParts)
	{
		PhysicsObject* part = new PhysicsObject(renderer, physicsEngine, false, true);
		part->GetRigidBody()->collider = new SphereCollider(1);
		part->AddMesh(*database->OBJMeshes->Find("../Data/meshes/robotcube.obj"));
		part->GetRigidBody()->parentMesh = part->GetSceneNode();
		part->SetPosition(modelpart->GetWorldTransform().GetPositionVector());
		part->SetSize(modelpart->GetModelScale());
		part->GetSceneNode()->SetColour(modelpart->GetColour());
		part->GetRigidBody()->gravity = -0.01f;
		part->GetRigidBody()->UpdateMass(100.0f);
		part->GetRigidBody()->drag = 0.9f;
		part->GetRigidBody()->atRest = false;
		part->GetSceneNode()->SetTransform(Matrix4::Translation(
			part->GetSceneNode()->GetTransform().GetPositionVector()) *
			/*playerRotation*/controller->GetCurrentRotation());

		bodies.push_back(part);
	}
}

void Player::Read(string resourcename) 
{
	this->SetResourceName(resourcename);
}

void Player::ReadParams(string params) 
{
	Read(params);
}
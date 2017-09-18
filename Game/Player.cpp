
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
	vars->insert({ "colourr", &colour.x });
	vars->insert({ "colourg", &colour.y });
	vars->insert({ "colourb", &colour.z });
	vars->insert({ "coloura", &colour.w });
	vars->insert({ "timer", &timer });
	vars->insert({ "collider", &collider });
	vars->insert({ "health", lifeSpan.GetCurrentHealth() });
	vars->insert({ "teamid", &teamid }); 
	vars->insert({ "killstreak", &killstreak });
	vars->insert({ "ypos", &rigidBody.lastPosition.y });

	idNumber = id;
	this->database = database;

	HUDText.SetNumberOfLinesInParagraph(2);
	HUDText.AddAdaptiveLine(AdaptiveLine(&rigidBody.tag, Vector3(30, 70, 0)));

	headsUpDisplay.SetRenderer(renderer);
	headsUpDisplay.SetText(&HUDText);

	ragdolls = new PlayerRagdollSet(2, playerModel, renderer, physicsEngine);

	this->SetResourceSize(sizeof(*this));
}

Player::~Player()
{
	delete playerModel;
	delete controller;
	delete gunInput;
	delete ragdolls;
}

void Player::ApplyInputs()
{
	if (!inputsLocked)
	{
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

	playerModel->UpdateColour(colour);
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

		MessageSystem::GetInstance()->StopTransmitting(Log::Hash(rigidBody.tag + "addkillstreakpoints")); 
	}
}

void Player::CheckGunChange()
{
	if (weaponChanger.RocketLauncherAvailable(rigidBody.tag))
	{
		gun = weaponChanger.CopyToRocketLauncher(gun);
		weaponChanger.AnnounceChange();
		weaponChanger.StopPreviousEvents();

		gunInput->SetWeapon(gun);
	}

	if (weaponChanger.MachineGunAvailable(rigidBody.tag))
	{
		gun = weaponChanger.CopyToMachineGun(gun);
		weaponChanger.AnnounceChange();
		weaponChanger.StopPreviousEvents();

		gunInput->SetWeapon(gun);
	}
}

void Player::PrepareHUD()
{
	HUDText.SetNumberOfLinesInParagraph(2);
	UpdateHUD();
}

void Player::UpdateHUD()
{
	string bulletsLeft = std::to_string(gun->bulletsPerMag - gun->bulletsFired);
	string maximumBullets = std::to_string(gun->bulletsPerMag);
	string ammoCount = bulletsLeft + "/" + maximumBullets;

	string timeUntilTeamChangeAvailable = std::to_string(5.0f - (std::max<float>)
		(0, ((std::min<float>)(this->timer, 5))));

	HUDText.SetLineOfParagraph(ammoCount, 0);
	HUDText.SetLineOfParagraph(timeUntilTeamChangeAvailable, 1);
}

void Player::DisplayHUD()
{
	UpdateHUD();

	HUDText.ClearAppendedText();

	if (gun->lastReloadTime + gun->reloadSpeed >= clock())
	{
		string isReloading = "reloading";
		HUDText.AppendLineToParagraph(isReloading);
	}

	headsUpDisplay.Set3DPosition(playerModel->GetTransform().GetPositionVector());
	headsUpDisplay.DisplayAllText();
}

void Player::Despawn()
{
	MessageSystem::GetInstance()->Transmit(Log::Hash(rigidBody.tag + "dead"), false);

	ragdolls->SpawnNextRagdoll(database, controller->GetCurrentRotation());

	//Move the player away from the screen and block inputs.
	rigidBody.UpdatePosition(rigidBody.lastPosition + Vector3(0, 100000, 0));
	rigidBody.gravity = 0;

	inputsLocked = true;
}

void Player::Respawn(const Vector3& spawnPoint)
{
	lifeSpan.ResetHealth();
	lifeSpan.ResetFramesSpentDead();

	inputsLocked = false;
	colour.w = 1;
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

void Player::UpdatePhysics(PhysicsEngine* newPhysicsEngine)
{
	physicsEngine = newPhysicsEngine;
	newPhysicsEngine->AddRigidBody(&rigidBody);

	ragdolls->SetPhysicsEngine(newPhysicsEngine);
}

void Player::UpdateRenderer(Renderer* newRenderer)
{
	renderer = newRenderer;
	newRenderer->AddSceneNode(rigidBody.parentMesh);

	ragdolls->SetRenderer(newRenderer);
	headsUpDisplay.SetRenderer(renderer);
}

void Player::SetPlayerController(PlayerController* im)
{
	controller = im;
}

CharacterModel* Player::GetPlayerModel() const
{
	return playerModel;
}

PlayerController* Player::GetPlayerController() const
{
	return controller;
}

RigidBody* Player::GetRigidBody()
{
	return &rigidBody;
}

void Player::AddSpawnPoint(const Vector3 newPoint)
{
	spawnPoints.push_back(newPoint);
}

const int Player::GetIDNumber() const
{
	return idNumber;
}

void Player::Read(string resourcename) 
{
	this->SetResourceName(resourcename);
}

void Player::ReadParams(string params) 
{
	Read(params);
}
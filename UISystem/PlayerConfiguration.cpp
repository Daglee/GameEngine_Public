#include "PlayerConfiguration.h"

#include "../Game/Player.h"
#include "../nclgl/Gamepad.h"
#include "GamepadMapper.h"
#include "MKMapper.h"
#include "../Game/TopDownController.h"
#include "../Game/Pistol.h"
#include "../Threading/ThreadPool.h"
#include "../Game/ScoreBoard.h"
#include "../Game/AudioManager.h"

PlayerConfiguration::PlayerConfiguration(Subsystems subsystems, BasePlayerSystems playerComponents)
{
	this->playerComponents = playerComponents;
	this->subsystems = subsystems;
}

PlayerConfiguration::~PlayerConfiguration()
{}

void PlayerConfiguration::ConnectPlayerToController(Player* player, Gamepad* gamepad)
{
	GamepadMapper* input = new GamepadMapper();
	input->SetGamePad(gamepad);

	player->SetPlayerController(new TopDownController(input));
	player->GetPlayerController()->SetInputMapper(input);

	player->gun = new Pistol(subsystems.database, subsystems.renderer,
		subsystems.physicsEngine, playerComponents.defaultBulletMesh);
}

void PlayerConfiguration::ConnectPlayerToKeyboard(Player* player)
{
	MKMapper* input = new MKMapper(subsystems.window, "../Data/ButtonMapping/MKMap.txt");
	player->SetPlayerController(new TopDownController(input));
	player->GetPlayerController()->SetInputMapper(input);

	subsystems.threadPool->pauseButton = input->PAUSE;

	player->gun = new Pistol(subsystems.database, subsystems.renderer,
		subsystems.physicsEngine, playerComponents.defaultBulletMesh);
}

void PlayerConfiguration::InitialisePlayer(Player* player, int playerNumber)
{
	SetSubsystems(player);
	SetBasicProperties(player, playerNumber);
	SetPlayerController(player);
	SetGameplayComponents(player);
}

void PlayerConfiguration::SetSubsystems(Player* player)
{
	player->GetPlayerModel()->UpdateMesh(playerComponents.defaultPlayerMesh);
	player->UpdatePhysics(subsystems.physicsEngine);
	player->UpdateRenderer(subsystems.renderer);
}

void PlayerConfiguration::SetBasicProperties(Player* player, int playerNumber)
{
	Vector3 spawnPos(-1 * (float)(50 * playerNumber), 100, 1 * (float)(50 * playerNumber));
	SpawnSystem::GetInstance()->AddSpawnPoint(spawnPos);

	player->GetRigidBody()->UpdatePosition(spawnPos);

	player->teamID = playerNumber; 

	string tag = "player" + std::to_string(playerNumber);
	player->GetRigidBody()->tag = tag;
	player->GetPlayerModel()->playertag = tag;
	player->walkingSoundName = "walkingsound" + tag;
	player->gun->parent = tag;

	player->playerModelMesh = playerComponents.defaultPlayerMesh;
}

void PlayerConfiguration::SetPlayerController(Player* player)
{
	player->GetPlayerController()->SetCharacterModel(player->GetPlayerModel());
	player->GetPlayerController()->SetRigidBody(player->GetRigidBody());
	player->GetPlayerController()->SetMovementSound(player->walkingSoundName);
}

void PlayerConfiguration::SetGameplayComponents(Player* player)
{
	player->gunInput = new GunInput(player->GetPlayerController()->GetInputMapper(), player->gun,
		WeaponData(&player->GetRigidBody()->lastPosition, player->GetIDNumber()));

	SoundNode* walkingSound = new SoundNode(
		new Sound("../Data/Sounds/41579__erdie__steps-on-stone01.wav"),
		player->GetPlayerModel());

	AudioManager::GetInstance()->AddSound("walkingsound" + player->GetRigidBody()->tag, walkingSound);

	ScoreBoard::GetInstance()->AddEntry(new Entry(player->GetRigidBody()->tag, 0));
}

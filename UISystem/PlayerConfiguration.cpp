#include "PlayerConfiguration.h"

#include "MKMapper.h"
#include "../nclgl/Gamepad.h"
#include "../Game/Player.h"
#include "../Game/TopDownController.h"
#include "../Game/Pistol.h"
#include "../Threading/ThreadPool.h"
#include "../Game/ScoreBoard.h"
#include "../Game/AudioManager.h"
#include "../Game/SpawnSystem.h"
#include "../Game/GunInput.h"

PlayerConfiguration::PlayerConfiguration(const BasePlayerSystems playerComponents)
{
	this->playerComponents = playerComponents;
}

PlayerConfiguration::~PlayerConfiguration()
{}

void PlayerConfiguration::InitialisePlayer(Player* player, const int playerNumber)
{
	SetBasicProperties(player, playerNumber);

	player->GetPlayerController()->UpdatePropertiesFromPlayer(player);

	SetGameplayComponents(player);
}

void PlayerConfiguration::SetBasicProperties(Player* player, const int playerNumber) const
{
	const Vector3 spawnPos(-1 * static_cast<float>(50 * playerNumber), 100, 1 * static_cast<float>(50 * playerNumber));
	SpawnSystem::GetInstance()->AddSpawnPoint(spawnPos);

	player->GetRigidBody()->UpdatePosition(spawnPos);

	player->teamID = playerNumber; 
	const string tag = "player" + to_string(playerNumber);

	player->GetRigidBody()->tag = tag;
	player->GetPlayerModel()->playertag = tag;
	player->walkingSoundName = "walkingsound" + tag;
	player->gun->parent = tag;

	player->GetPlayerModel()->UpdateMesh(playerComponents.defaultPlayerMesh);
	player->gun->bulletMesh = playerComponents.defaultBulletMesh;
	player->playerModelMesh = playerComponents.defaultPlayerMesh;
}

void PlayerConfiguration::SetGameplayComponents(Player* player) const
{
	player->gunInput = new GunInput(player->GetPlayerController()->GetInputMapper(), player->gun,
		WeaponData(&player->GetRigidBody()->lastPosition, player->GetIDNumber()));

	SoundNode* walkingSound = new SoundNode(
		new Sound("../Data/Sounds/41579__erdie__steps-on-stone01.wav"),
		player->GetPlayerModel());

	AudioManager::GetInstance()->AddSound("walkingsound" + player->GetRigidBody()->tag, walkingSound);

	ScoreBoard::GetInstance()->AddEntry(new Entry(player->GetRigidBody()->tag, 0));
}

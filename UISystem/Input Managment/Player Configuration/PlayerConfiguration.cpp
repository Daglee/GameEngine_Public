#include "PlayerConfiguration.h"

#include "../../MouseAndKeyboard/MKMapper.h"
#include "../nclgl/InputDevices/Gamepad.h"
#include "../Game/Players/Player.h"
#include "../Game/Players/Control/TopDownController.h"
#include "../Game/Gameplay/Weapon Types/Pistol.h"
#include "../Threading/ThreadPool/ThreadPool.h"
#include "../Game/Scoring/ScoreBoard.h"
#include "../Game/Audio/AudioManager.h"
#include "../Game/Gameplay/Spawning/SpawnSystem.h"
#include "../Game/Gameplay/Weapon Control/GunInput.h"

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

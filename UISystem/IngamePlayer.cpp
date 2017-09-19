#include "IngamePlayer.h"

#include "../Game/Player.h"
#include "../Game/Pistol.h"

IngamePlayer::IngamePlayer(Subsystems subsystems)
{
	this->subsystems = subsystems;
}

IngamePlayer::~IngamePlayer()
{}

void IngamePlayer::AddPlayerToGame(Player* player)
{
	player->gun = new Pistol(subsystems.database, subsystems.renderer,
		subsystems.physicsEngine, nullptr);

	player->UpdatePhysics(subsystems.physicsEngine);
	player->UpdateRenderer(subsystems.renderer);
}

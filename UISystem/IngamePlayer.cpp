#include "IngamePlayer.h"

#include "../Game/Player.h"
#include "../Game/Pistol.h"

IngamePlayer::IngamePlayer(const Subsystems subsystems)
{
	this->subsystems = subsystems;
}

IngamePlayer::~IngamePlayer()
{}

void IngamePlayer::AddPlayerToGame(Player* player) const
{
	player->gun = new Pistol(subsystems.database, subsystems.renderer,
		subsystems.physicsEngine, nullptr);

	player->UpdatePhysics(subsystems.physicsEngine);
	player->UpdateRenderer(subsystems.renderer);
}

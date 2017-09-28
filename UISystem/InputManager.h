#pragma once

#include "MKMapper.h"
#include "../Game/Players/Player.h"
#include <vector>
#include "../Game/Subsystem.h"
#include "../ResourceManagment/Resource.h"

#include "Playerbase.h"

class DataBase;
class ThreadPool;

/*
  Manages input devices and their connections to players.
  Player 1 will always use M&K. The other 4 will use a controller
*/
class InputManager : public Subsystem, public Resource
{
public:
	InputManager(Playerbase* playerbase, Window* window);
	~InputManager();

	void Update(const float& deltatime) override;

	Playerbase* GetPlayerbase() const
	{
		return playerbase;
	}

	void Read(const string resourcename) override;
	void ReadParams(const string params) override;

private:
	std::vector<Player*>* connectedPlayers;
	Playerbase* playerbase;
	Window* window;
};


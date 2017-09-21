#pragma once

#include "../nclgl/Gamepad.h"
#include "GamepadMapper.h"
#include "MKMapper.h"
#include "../Game/Player.h"
#include <vector>
#include "../Game/Subsystem.h"
#include "../ResourceManagment/Resource.h"
#include "PlayerConfiguration.h"
#include "IngamePlayer.h"

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

	/*
	  Fill the input arrays/buffers...
	  and apply them!
	*/
	void Update(float deltatime);

	Playerbase* GetPlayerbase()
	{
		return playerbase;
	}

	void Read(string resourcename);
	void ReadParams(string params);

private:
	std::vector<Player*>* connectedPlayers;
	Playerbase* playerbase;
	Window* window;
};


#pragma once

#include "IngamePlayer.h"
#include "PlayerConfiguration.h"

#include <vector>
#include <functional>

class Player;
class Gamepad;
class DataBase;

class Playerbase
{
public:
	Playerbase(DataBase* database);
	~Playerbase();

	void RetrieveGamepadsAndPlayers(DataBase* db);
	void ConnectGamepads(bool reconnection);
	void UpdateConnections();

	void ReInitialisePlayers();

	void ClearAll();

	std::vector<Player*>* GetConnectedPlayers()
	{
		return &connectedPlayers;
	}

private:
	void StoreDatabase();
	void AddPlayerToGame(Player* player);

	//void StoreGamepadPlayer(int index);
	void StoreGamepad(int index);
	void StorePlayer(int index);

	std::vector<Gamepad*> connectedGPads;
	std::vector<Player*> connectedPlayers;

	Gamepad** gamepads;
	Player** players;

	DataBase* database;
	PlayerConfiguration* playerConfig;
	Subsystems systems;
};


#pragma once

#include "../Player Configuration/IngamePlayer.h"
#include "../Player Configuration/PlayerConfiguration.h"

#include <vector>

class Player;
class Gamepad;
class DataBase;

class Playerbase
{
public:
	Playerbase(DataBase* database);
	~Playerbase();

	void RetrieveGamepadsAndPlayers(DataBase* db);
	void ConnectGamepads(const bool reconnection);
	void UpdateConnections();

	void ReInitialisePlayers();

	void ClearAll();

	std::vector<Player*>* GetConnectedPlayers()
	{
		return &connectedPlayers;
	}

private:
	void StoreDatabase();
	void AddPlayerToGame(Player* player) const;

	void StoreGamepad(const int index);
	void StorePlayer(const int index);

	std::vector<Gamepad*> connectedGPads;
	std::vector<Player*> connectedPlayers;

	Gamepad** gamepads;
	Player** players;

	DataBase* database;
	PlayerConfiguration* playerConfig;
	Subsystems systems;
};


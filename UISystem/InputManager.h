#pragma once

#include "../nclgl/Gamepad.h"
#include "GamepadMapper.h"
#include "MKMapper.h"
#include "../Game/Player.h"
#include <vector>
#include "../Game/Subsystem.h"
#include "../ResourceManagment/ResourceBase.h"
#include "PlayerConfiguration.h"

class DataBase;
class ThreadPool;

/*
  Manages input devices and their connections to players.
  Player 1 will always use M&K. The other 4 will use a controller
*/
class InputManager : public Subsystem, public ResourceBase
{
public:
	InputManager(ThreadPool* t);
	~InputManager();

	//Return the number of controllers connected -- max 6
	std::vector<Gamepad*>* ConnectGamepads(bool reconnection);

	//Players match up with controller. eg Gamepad2 = Player2
	std::vector<Player*>* GetConnectedPlayers()
	{
		return &connectedPlayers;
	}

	//Fill collections with what is loaded for the current level/game
	void ConnectToDataBase(DataBase* databaseToConnect);

	/*
	  Check if the players that were previously connected, still are.
	  Can be done selectively, rather than polling
	  every frame.
	*/
	void UpdateConnections();
	void ReInitialisePlayers();

	/*
	  Fill the input arrays/buffers...
	  and apply them!
	*/
	void Update(float deltatime);

	void ClearAll();

	void Read(string resourcename);
	void ReadParams(string params);

	ThreadPool* threadPool;

private:
	void StoreDatabase();
	void StoreGamepadPlayer(int index);

	Gamepad** gamepads;
	Player** players;
	DataBase* database;
	Window* window;

	//Previously connected gamepads (updated on last ConnectedGamePads() or UpdateConnections() call)
	std::vector<Gamepad*> connectedGPads;
	//Previously connected players (updated on last ConnectedGamePads() or UpdateConnections() call)
	std::vector<Player*> connectedPlayers;

	/*
	  These are stored to save the database from
	  searching for them on every additional player.
	*/
	//Mesh* defaultBulletMesh;
	//Mesh* defaultPlayerMesh;
	//Renderer* renderer;
	//PhysicsEngine* physicsEngine;

	PlayerConfiguration* playerConfig;
};


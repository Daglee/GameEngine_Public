#pragma once

#include "../nclgl/Gamepad.h"
#include "GamepadMapper.h"
#include "MKMapper.h"
#include "../Game/Player.h"
#include <vector>
#include "../Game/Subsystem.h"
#include "../ResourceManagment/ResourceBase.h"

class DataBase;

/*
  Manages input devices and their connections to players.
  Player 1 will always use M&K. The other 4 will use a controller
*/
class InputManager : public Subsystem, public ResourceBase
{
public:
	InputManager();
	~InputManager();

	//Return the number of controllers connected -- max 6
	std::vector<Gamepad*>* ConnectedGamePads(); 

	//Players match up with controller. eg Gamepad2 = Player2
	std::vector<Player*>* GetConnectedPlayers()
	{
		return &connectedPlayers;
	}

	//Fill collections with what is loaded for the current level/game
	void ConnectToDataBase(DataBase* db);

	/*
	  Check if the players that were previously connected, still are.
	  Can be done selectively, rather than polling
	  every frame.
	*/
	void UpdateConnections();

	/*
	  Fill the input arrays/buffers...
	  and apply them!
	*/
	void Update(float deltatime);

	void Read(string resourcename);
	void ReadParams(string params);

protected:
	//For players using a controller
	void InitialisePlayer(Player* p, Gamepad* gp);

	//For player 1 - which uses M&K
	void InitialisePlayer(Player* p);

	//General set of parameters used for both types of player
	void SetPlayerParameters(Player* p);

	Gamepad** gpad;
	Player** players;
	DataBase* database;
	Window* window;

	//Previously connected gamepads (updated on last ConnectedGamePads() or UpdateConnections() call)
	std::vector<Gamepad*> connectedGPads; 
	//Previously connected players (updated on last ConnectedGamePads() or UpdateConnections() call)
	std::vector<Player*> connectedPlayers;

private:
	/*
	  These are stored to save the database from 
	  searching for them on every additional player.
	*/
	Mesh* defaultBulletMesh;
	Mesh* defaultPlayerMesh;
	Renderer* renderer;
	PhysicsEngine* physicsEngine;
};


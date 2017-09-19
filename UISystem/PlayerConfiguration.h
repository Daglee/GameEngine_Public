#pragma once

class Player;
class Gamepad;
class Mesh;
class Renderer;
class PhysicsEngine;
class DataBase;
class ThreadPool;
class Window;

struct BasePlayerSystems
{
	Mesh* defaultBulletMesh;
	Mesh* defaultPlayerMesh;
};

struct Subsystems
{
	DataBase* database;
	Renderer* renderer;
	PhysicsEngine* physicsEngine;
	ThreadPool* threadPool;
	Window* window;
};

class PlayerConfiguration
{
public:
	PlayerConfiguration(Subsystems subsystems, BasePlayerSystems playerComponents);
	~PlayerConfiguration();

	void ConnectPlayerToController(Player* player, Gamepad* gamepad);
	void ConnectPlayerToKeyboard(Player* player);

	void InitialisePlayer(Player* player, int playerNumber);

private:
	void SetSubsystems(Player* player);
	void SetBasicProperties(Player* player, int playerNumber);
	void SetPlayerController(Player* player);
	void SetGameplayComponents(Player* player);

	BasePlayerSystems playerComponents;
	Subsystems subsystems;
};


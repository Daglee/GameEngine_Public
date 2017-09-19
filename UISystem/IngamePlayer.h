#pragma once

class Player;
class Renderer;
class PhysicsEngine;
class DataBase;
class ThreadPool;
class Window;

struct Subsystems
{
	DataBase* database;
	Renderer* renderer;
	PhysicsEngine* physicsEngine;
	ThreadPool* threadPool;
	Window* window;
};

class IngamePlayer
{
public:
	IngamePlayer(Subsystems subsystems);
	~IngamePlayer();

	void AddPlayerToGame(Player* player);

private:
	Subsystems subsystems;
};


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
	explicit IngamePlayer(const Subsystems subsystems);
	~IngamePlayer();

	void AddPlayerToGame(Player* player) const;

private:
	Subsystems subsystems;
};


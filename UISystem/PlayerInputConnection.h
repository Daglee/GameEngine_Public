#pragma once

class Player;
class Gamepad;
class Window;
class ThreadPool;
class InputMapper;

class PlayerInputConnection
{
public:
	PlayerInputConnection(Player* player);
	~PlayerInputConnection() {}

	void ConnectPlayerToController(Gamepad* gamepad);
	void ConnectPlayerToKeyboard(Window* window, ThreadPool* threadPool);

private:
	void AssignController(InputMapper* input);

	Player* player;
};


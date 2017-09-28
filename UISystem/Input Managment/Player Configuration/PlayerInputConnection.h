#pragma once

class Player;
class Gamepad;
class Window;
class ThreadPool;
class InputMapper;

class PlayerInputConnection
{
public:
	explicit PlayerInputConnection(Player* player);
	~PlayerInputConnection() {}

	void ConnectPlayerToController(Gamepad* gamepad) const;
	void ConnectPlayerToKeyboard(Window* window, ThreadPool* threadPool) const;

private:
	void AssignController(InputMapper* input) const;

	Player* player;
};


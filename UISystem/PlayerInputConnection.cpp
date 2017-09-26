#include "PlayerInputConnection.h"

#include "../Game/Player.h"
#include "GamepadMapper.h"
#include "MKMapper.h"
#include "../Game/TopDownController.h"
#include "../Threading/ThreadPool.h"
#include "InputMapper.h"

PlayerInputConnection::PlayerInputConnection(Player* player)
{
	this->player = player;
}

void PlayerInputConnection::ConnectPlayerToController(Gamepad* gamepad) const
{
	GamepadMapper* input = new GamepadMapper();
	input->SetGamePad(gamepad);

	AssignController(input);
}

void PlayerInputConnection::ConnectPlayerToKeyboard(Window* window, ThreadPool* threadPool) const
{
	MKMapper* input = new MKMapper(window, "../Data/ButtonMapping/MKMap.txt");
	AssignController(input);

	threadPool->pauseButton = input->PAUSE;
}

void PlayerInputConnection::AssignController(InputMapper* input) const
{
	player->SetPlayerController(new TopDownController(input));
	player->GetPlayerController()->SetInputMapper(input);
}

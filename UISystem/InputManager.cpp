#include "InputManager.h"
#include "../ResourceManagment/DataBase.h"
#include "../Threading/ThreadPool.h"
#include "../Game/ScoreBoard.h"
#include "../Game/Pistol.h"
#include "../Game/RocketLauncher.h"
#include "../Game/TopDownController.h"
#include "../Game/SpawnSystem.h"
#include "PlayerInputConnection.h"

InputManager::InputManager(Playerbase* playerbase, Window* window)
{
	this->window = window;
	this->playerbase = playerbase;

	this->connectedPlayers = playerbase->GetConnectedPlayers();

	SpawnSystem::Initialise();
}

InputManager::~InputManager()
{
	delete playerbase;
}

//Fill the buffers and use them!
void InputManager::Update(float deltatime)
{
	updateTimer.StartTimer();

	for (vector<Player*>::iterator player = connectedPlayers->begin();
		player != connectedPlayers->end(); player++)
	{
		(*player)->Update(deltatime, window->GetTimer()->GetMS());
		(*player)->GetPlayerController()->GetInputMapper()->ClearInputs();
		(*player)->GetPlayerController()->GetInputMapper()->FillInputs();
		(*player)->ApplyInputs();
	}

	updateTimer.StopTimer();
}

void InputManager::Read(string resourcename)
{
	this->SetName(resourcename);
}

void InputManager::ReadParams(string params)
{
	Read(params);
}
#include "InputManager.h"
#include "../Threading/ThreadPool.h"
#include "../Game/TopDownController.h"
#include "../Game/SpawnSystem.h"

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
void InputManager::Update(const float& deltatime)
{
	updateTimer.StartTimer();

	for (vector<Player*>::iterator player = connectedPlayers->begin();
		player != connectedPlayers->end(); ++player)
	{
		(*player)->Update(deltatime, window->GetTimer()->GetMS());
		(*player)->GetPlayerController()->GetInputMapper()->ClearInputs();
		(*player)->GetPlayerController()->GetInputMapper()->FillInputs();
		(*player)->ApplyInputs();
	}

	updateTimer.StopTimer();
}

void InputManager::Read(const string resourcename)
{
	this->SetName(resourcename);
}

void InputManager::ReadParams(const string params)
{
	Read(params);
}
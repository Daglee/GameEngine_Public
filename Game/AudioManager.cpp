#include "AudioManager.h"

#include "../nclgl/SoundSystem.h"
#include "../ResourceManagment/DataBase.h"

AudioManager* AudioManager::instance = NULL;

AudioManager::AudioManager(SceneNode* camNode) : Resource()
{
	//The SoundSystem singleton is managed in this class.
	SoundSystem::Initialise();
	SoundSystem::GetSoundSystem()->SetListener(camNode);

	this->SetSize(sizeof(*instance) + sizeof(*SoundSystem::GetSoundSystem()));
}

AudioManager::AudioManager() : Resource()
{
	SoundSystem::Initialise();

	this->SetSize(sizeof(*instance) + sizeof(*SoundSystem::GetSoundSystem()));
}

AudioManager::~AudioManager()
{
	SoundSystem::Destroy();

	ClearRemoveBuffer();

	for each (std::pair<std::string, SoundNode*> p in sounds)
	{
		delete p.second;
	}
	sounds.clear();

	Destroy();
}

void AudioManager::BeginPlay(SoundNode* gs)
{
	/*
	  Is the update busy? If not, add. If it is,
	  then just wait.
	*/
	unique_lock<mutex> lock(update_mutex);

	gs->SetLooping(true);
	SoundSystem::GetSoundSystem()->AddSoundNode(gs);
}

void AudioManager::BeginBackgroundPlay(string name)
{
	SoundNode* sound = backgroundSounds.at(name);

	if (sound == nullptr) Log::Error("Sound " + name + " not found.");

	sound->SetLooping(true);
	sound->SetIsGlobal(true);
	SoundSystem::GetSoundSystem()->AddSoundNode(sound);
}

void AudioManager::BeginPlay(string name)
{
	BeginPlay(GetSound(name));
}

void AudioManager::StopPlay(SoundNode* gs)
{
	/*
	  Is the update busy? If not, add. If it is,
	  then just wait.
	*/
	unique_lock<mutex> lock(update_mutex);

	if (gs->GetLooping())
	{ //If currently playing...
		gs->SetLooping(false);
		gs->DetachSource();
	}
}

void AudioManager::StopPlay(string name)
{
	StopPlay(GetSound(name));
}

void AudioManager::PlayOnce(SoundNode* gs)
{
	SoundSystem::GetSoundSystem()->AddSoundNode(gs);
}

void AudioManager::PlayOnce(string name)
{
	PlayOnce(GetSound(name));
}

void AudioManager::TemporaryPlay(Sound* gs, enum SoundPriority sp)
{
	remove_buffer.push_back(gs);
	SoundSystem::GetSoundSystem()->PlaySound(gs, sp);
	this->SetSize(sizeof(*instance) + sizeof(*SoundSystem::GetSoundSystem()));
}

void AudioManager::TemporaryPlay(string name, enum SoundPriority sp)
{
	TemporaryPlay(GetSound(name)->GetSound(), sp);
}

void AudioManager::Update(float deltatime)
{
	/*
	  LOCK! Nothing in the engine is allowed
	  to change until the update is done.
	*/
	lock_guard<mutex> lock(update_mutex);

	updateTimer.StartTimer();

	ClearRemoveBuffer();
	this->SetSize(sizeof(*instance) + sizeof(*SoundSystem::GetSoundSystem()));

	//Play the audio!
	for each (std::pair<string, SoundNode*> bsound in backgroundSounds)
	{
		BeginBackgroundPlay(bsound.first);
	}

	SoundSystem::GetSoundSystem()->Update(deltatime);

	updateTimer.StopTimer();
}

void AudioManager::ClearRemoveBuffer()
{
	vector<Sound*>::iterator iterator = remove_buffer.begin();
	while (iterator != remove_buffer.end())
	{
		if ((*iterator)->sound->IsStreaming())
		{ //Is it currently playing?
			delete *iterator;
			iterator = remove_buffer.erase(iterator);
		}
		else ++iterator;
	}
}

void AudioManager::Read(string resourcename)
{
	SetName(resourcename);
}

void AudioManager::ReadParams(string params)
{
	Read(params);
}
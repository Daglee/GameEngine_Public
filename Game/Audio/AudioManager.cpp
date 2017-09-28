#include "AudioManager.h"

#include "../../nclgl/Sound/SoundSystem.h"
#include "../../ResourceManagment/Database/DataBase.h"

AudioManager* AudioManager::instance = NULL;

AudioManager::AudioManager(SceneNode* camNode) : Resource()
{
	SoundSystem::Initialise();
	SoundSystem::GetSoundSystem()->SetListener(camNode);

	this->SetSizeInBytes(sizeof(*instance) + sizeof(*SoundSystem::GetSoundSystem()));
}

AudioManager::AudioManager() : Resource()
{
	SoundSystem::Initialise();

	this->SetSizeInBytes(sizeof(*instance) + sizeof(*SoundSystem::GetSoundSystem()));
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

void AudioManager::BeginPlay(SoundNode* sound)
{
	unique_lock<mutex> lock(updateMutex);

	sound->SetLooping(true);
	SoundSystem::GetSoundSystem()->AddSoundNode(sound);
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
	unique_lock<mutex> lock(updateMutex);

	if (gs->GetLooping())
	{
		gs->SetLooping(false);
		gs->DetachSource();
	}
}

void AudioManager::StopPlay(string name)
{
	StopPlay(GetSound(name));
}

void AudioManager::PlayOnce(SoundNode* sound)
{
	SoundSystem::GetSoundSystem()->AddSoundNode(sound);
}

void AudioManager::PlayOnce(string name)
{
	PlayOnce(GetSound(name));
}

void AudioManager::TemporaryPlay(Sound* sound, enum SoundPriority priority)
{
	remove_buffer.push_back(sound);
	SoundSystem::GetSoundSystem()->PlaySound(sound, priority);
	this->SetSizeInBytes(sizeof(*instance) + sizeof(*SoundSystem::GetSoundSystem()));
}

void AudioManager::TemporaryPlay(string name, enum SoundPriority priority)
{
	TemporaryPlay(GetSound(name)->GetSound(), priority);
}

void AudioManager::Update(const float& deltatime)
{
	lock_guard<mutex> lock(updateMutex);

	updateTimer.StartTimer();

	ClearRemoveBuffer();
	this->SetSizeInBytes(sizeof(*instance) + sizeof(*SoundSystem::GetSoundSystem()));

	for each (pair<string, SoundNode*> bsound in backgroundSounds)
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
		{
			delete *iterator;
			iterator = remove_buffer.erase(iterator);
		}
		else ++iterator;
	}
}

void AudioManager::Read(const string resourcename)
{
	SetName(resourcename);
}

void AudioManager::ReadParams(const string params)
{
	Read(params);
}
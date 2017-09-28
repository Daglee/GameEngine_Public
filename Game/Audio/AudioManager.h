#pragma once

#include "../../ResourceManagement/Resources/Resource.h"
#include "../../ResourceManagement/Utilities/Log.h"
#include "../../nclgl/Sound/SoundNode.h"
#include "../../Game/Subsystems/Subsystem.h"

#include <unordered_map>
#include <mutex>

/*
  Wrapper around SoundSystem.
*/
class AudioManager : public Resource,
	public Subsystem
{
public:
	static void Initialise(SceneNode* camNode)
	{
		instance = new AudioManager(camNode);
	}

	static void Initialise()
	{
		instance = new AudioManager();
	}

	static AudioManager* GetInstance()
	{
		return instance;
	}

	/*
	  SoundNode -- contains Sound object that will be played.
	  Name		-- For the hash map + retrieval.
	*/
	void AddSound(string name, SoundNode* sound)
	{
		if (sound == nullptr)
		{
			Log::Error("SoundNode " + name + " is null. Cannot be added.");
		}
		else
		{
			sounds.insert({ name, sound });
		}

		this->SetSizeInBytes(sizeof(*instance));
	}

	void AddBackgroundSound(string filename)
	{
		SoundNode* backgroundSound = new SoundNode(
			new Sound(filename), new SceneNode());

		backgroundSounds.insert({ filename, backgroundSound });

		this->SetSizeInBytes(sizeof(*instance));
	}

	inline SoundNode* GetSound(string name)
	{
		SoundNode* sound = sounds.at(name);

		if (sound == nullptr)
		{
			Log::Error("Sound " + name + " not found.");
		}

		return sounds.at(name);
	}

	void BeginPlay(string name); 
	void StopPlay(string name);  
	void PlayOnce(string name);  

	void BeginBackgroundPlay(string name);
	
	void TemporaryPlay(string name, enum SoundPriority sp);
	void TemporaryPlay(Sound* sound, enum SoundPriority sp);

	void SetListener(SceneNode* camNode)
	{
		SoundSystem::GetSoundSystem()->SetListener(camNode);
	}

	void Update(const float& deltatime) override;

	void Read(const string resourcename) override;
	void ReadParams(const string params) override;

protected:
	AudioManager(SceneNode* camNode);
	AudioManager();

	virtual ~AudioManager();
	static void Destroy()
	{
		delete instance;
	}

	void BeginPlay(SoundNode* sound);
	void StopPlay(SoundNode* sound);
	void PlayOnce(SoundNode* sound);

	void ClearRemoveBuffer();

	unordered_map<string, SoundNode*> sounds;
	unordered_map<string, SoundNode*> backgroundSounds;

	vector<Sound*> remove_buffer;

	static AudioManager* instance;
	mutex updateMutex;
};


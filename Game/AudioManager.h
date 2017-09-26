#pragma once

#include "../ResourceManagment/Resource.h"
#include "../ResourceManagment/Log.h"
#include "../nclgl/SoundNode.h"
#include "../Game/Subsystem.h"

#include <unordered_map>
#include <mutex>

/*
  Singleton AudioManager that acts as a wrapper
  around SoundSystem.

  Maintains a record of all sounds that are going
  to be played/are stored. Use of hashmap ensures
  fast retrieval.
*/
class AudioManager : public Resource,
	public Subsystem
{
public:
	/*
	  A mutex is needed in the update function to make sure
	  no sounds are added or played during the update due
	  to another thread.

	  Performance loss is negligible and it stops a whole
	  load of errors.
	  Exactly the same as the one in the physics engine.
	*/
	mutex update_mutex;

	//If the camera has already been set up...
	static void Initialise(SceneNode* camNode)
	{
		instance = new AudioManager(camNode);
	}

	//If the camera isnt ready yet...
	static void Initialise()
	{
		instance = new AudioManager();
	}

	//Grants you access to all of this!
	static AudioManager* GetInstance()
	{
		return instance;
	}

	/*
	  SoundNode -- contains Sound object that will be played.
	  Name		-- For the hash map + retrieval.
	*/
	void AddSound(string name, SoundNode* gs)
	{
		if (gs == nullptr) Log::Error("SoundNode " + name + " is null. Cannot be added.");
		else sounds.insert({ name, gs });

		this->SetSizeInBytes(sizeof(*instance));
	}

	void AddBackgroundSound(string filename)
	{
		SoundNode* backgroundSound = new SoundNode(
			new Sound(filename), new SceneNode());

		backgroundSounds.insert({ filename, backgroundSound });

		this->SetSizeInBytes(sizeof(*instance));
	}

	//Get the sound by name to do things with it...
	inline SoundNode* GetSound(string name)
	{
		SoundNode* sound = sounds.at(name);

		if (sound == nullptr)
		{
			Log::Error("Sound " + name + " not found.");

			return nullptr;
		}
		else return sounds.at(name);
	}

	/*
	  Passing a SoundNode is not allowed, therefore
	  disallowing SoundNodes to be made on the heap,
	  tampering with memory managment.
	*/
	void BeginPlay(string name); //Start playing until told to stop...
	void StopPlay(string name);  //Stop playing if it is already playing.
	void PlayOnce(string name);  //Just play the sound once.

	void BeginBackgroundPlay(string name);
	/*
	  Used, for example, for gun shots.
	  Allows multiples of a single sound to be played,
	  layering on top of eachother.

	  Will insert the sounds into a buffer that will
	  remove them once playback has finished, freeing
	  any memory that they took up.

	  Please look inside the ApplyInputs function
	  of the Player class for an example of usage.
	*/
	void TemporaryPlay(string name, enum SoundPriority sp);
	void TemporaryPlay(Sound* gs, enum SoundPriority sp);

	void SetListener(SceneNode* camNode)
	{
		SoundSystem::GetSoundSystem()->SetListener(camNode);
	}

	//Updates sound system and temporary sounds buffer.
	void Update(const float& deltatime) override;

	void Read(const string resourcename) override;
	void ReadParams(const string params) override;

protected:
	AudioManager(SceneNode* camNode);
	AudioManager(void);

	virtual ~AudioManager();
	static void Destroy()
	{
		delete instance;
	}

	void BeginPlay(SoundNode* gs);
	void StopPlay(SoundNode* gs);
	void PlayOnce(SoundNode* gs);

	void ClearRemoveBuffer();

	//Stores all sounds that will be played at multiple points...
	unordered_map<std::string, SoundNode*> sounds;
	unordered_map<std::string, SoundNode*> backgroundSounds;

	//Temporary sounds stored here from TemporaryPlay
	vector<Sound*> remove_buffer;

	//Singleton.
	static AudioManager* instance;
};


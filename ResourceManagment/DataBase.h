#pragma once

#include "ResourceManager.h"
#include "../Profiler/MemoryWatcher.h"
#include "../Threading/ThreadPool.h"
#include "../Game/PhysicsObject.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/OBJMesh.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../GameLogicFSM/FSM.h"
#include "../NCLGL/Renderer.h"
#include "../nclgl/Gamepad.h"
#include "../nclgl/Camera.h"
#include "../Physics/PhysicsEngine.h"
#include "../nclgl/Window.h"
#include "../Game/Player.h"
#include "../Profiler/Profiler.h"
#include "../UISystem/InputManager.h"
#include "../Game/AudioManager.h"

#include <unordered_map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <thread>
#include <mutex>
#include <ctime>

/*
  Contains a series of ResourceManagers - or...tables.
  Using ResourceManagers that only take in one type has benefits such as:
   -- Little or no need to defragment memory.
   -- Extremely fast retrieval through hashing.
   -- No runtime type identification such as dynamic cast required 
      to retrieve an object. The pointer of that type is returned.
   -- More specific control over memory usage available.

   Essentially, it's built to be incredibly fast, rather than incredibly flexible.
*/
class DataBase
{
public:
	typedef void(DataBase::*Function)(string);
	typedef std::unordered_map<std::string, Function> FunctionMap;
	/*
	  Contains the functions that will add a resource to a resource manager.
	  Allows one specific resource manager to be added to via a string.
	*/
	FunctionMap addmap;

	DataBase();
	~DataBase();

	void InitialiseDataBase();

	void AddToResourceManager(string manager, string resourceName);

	void StartUp(string filename, bool rendererInitialised, bool renderLoadingScreen);

	size_t CurrentSize() const;
	size_t MaxSize() const;

	ResourceManager<ThreadPool>*	GThreadPool;
	ResourceManager<Renderer>*		GRenderer;
	ResourceManager<PhysicsEngine>*	GPhysicsEngine;
	ResourceManager<Window>*		GWindow;
	ResourceManager<OBJMesh>*		OBJMeshes;
	ResourceManager<Mesh>*			Meshes;
	ResourceManager<PhysicsObject>* PhysicsObjects;
	ResourceManager<Camera>*		GCamera;
	ResourceManager<Player>*		Players;
	ResourceManager<Gamepad>*		Gamepads;
	ResourceManager<FSMManager>*	GFSMManager;
	ResourceManager<Profiler>*		GProfiler;
	ResourceManager<InputManager>*	GInputManager;
	ResourceManager<AudioManager>*	GAudioManager;
	ResourceManager<GameObject>*	GameObjects;
private:
	void InitialiseFunctionMap();

	//The following functions are used for start up.
	void ReadLoop(vector<vector<string>> tokens, vector<string> line, int numLines);

	void ReadItem(vector<string> tokens, string line);
	void ReadParameters(vector<string> tokens, string line,
		bool* renderInit, bool renderLoadingScreen);

	void AddToGRenderer		(string params);
	void AddToGProfiler		(string params);
	void AddToPhysicsEngine	(string params);
	void AddToGFSMManager	(string params);
	void AddToGCamera		(string params);
	void AddToGThreadPool	(string params);
	void AddToGWindow		(string params);
	void AddToPhysicsObjects(string resourceName);
	void AddToGInputManager	(string resourcename);
	void AddToGamepads		(string resourcename);
	void AddToPlayers		(string resourceName);
	void AddToOBJMeshes		(string meshLocation);
	void AddToMeshes		(string meshLocation);
	void AddToGameObjects	(string resourceName);

	int playerIDCount;
};


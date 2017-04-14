#pragma once
#include "DatabaseIncludes.h"
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

#include "../Threading/ThreadPool.h"
#include "../Profiler/MemoryWatcher.h"
#include "ResourceManager.h"
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

#define MAX_PHYSICS_OBJ_PER_BIN 5
#define MAX_OBJMESH_PER_BIN 300
#define MAX_PHYSICS_OBJ_BINS 30
#define MAX_PLAYERS 6
#define SINGLE 1

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

	DataBase()
	{
		//No runtime type identification needed!
		GThreadPool = new ResourceManager<ThreadPool>(false, true);
		PhysicsObjects = new ResourceManager<PhysicsObject>(false, true);
		OBJMeshes = new ResourceManager<OBJMesh>(false, true);
		GRenderer = new ResourceManager<Renderer>(false, true);
		GPhysicsEngine = new ResourceManager<PhysicsEngine>(false, true);
		GWindow = new ResourceManager<Window>(false, true);
		GCamera = new ResourceManager<Camera>(false, true);
		Players = new ResourceManager<Player>(false, true);
		Gamepads = new ResourceManager<Gamepad>(false, true);
		GFSMManager = new ResourceManager<FSMManager>(false, true);
		GProfiler = new ResourceManager<Profiler>(false, true);
		GInputManager = new ResourceManager<InputManager>(false, true);
		GAudioManager = new ResourceManager<AudioManager>(false, true);
		InitialiseFunctionMap();
	}

	~DataBase() {
		delete GThreadPool;
		delete PhysicsObjects;
		delete OBJMeshes;
		delete GRenderer;
		delete GPhysicsEngine;
		delete GWindow;
		delete GCamera;
		delete Players;
		delete Gamepads;
		delete GFSMManager;
		delete GProfiler;
		delete GInputManager;
		delete GAudioManager;
	}

	void InitialiseDataBase() {
		size_t size = 5000;

		GThreadPool->Initialise("GThreadPool", size_t(10000), SINGLE);
		PhysicsObjects->Initialise("Physics Object Manager", size_t(10000), 5);
		OBJMeshes->Initialise("OBJMesh Manager", size_t(10000), 5);
		GRenderer->Initialise("Renderer Manager", size_t(10000), SINGLE);
		GPhysicsEngine->Initialise("Physics Engine Manager", size_t(10000), SINGLE);
		GWindow->Initialise("Window Manager", size_t(10000), SINGLE);
		GCamera->Initialise("Camera", size_t(10000), SINGLE);
		Players->Initialise("Players", size_t(10000), SINGLE);
		Gamepads->Initialise("Gamepads", size_t(10000), SINGLE);
		GFSMManager->Initialise("GFSMManager", size_t(10000), SINGLE);
		GProfiler->Initialise("GProfiler", size_t(10000), SINGLE);
		GInputManager->Initialise("GInputManager", size_t(10000), SINGLE);
		GAudioManager->Initialise("GAudiomanager", size_t(10000), SINGLE);

		GAudioManager->Load("AudioManager", AudioManager::GetInstance());

		size_t databaseSize = MaxSize();
	}

	void AddToResourceManager(string manager, string resourceName) {
		FunctionMap::iterator iter = addmap.find(manager);
		if (iter != addmap.end()) {
			//Found
			(this->*(iter->second))(resourceName);
		}
	}

	void DataBase::StartUp(string filename, bool rendererInitialised, bool renderLoadingScreen) {
		std::ifstream file(filename);
		string line;

		bool renderInit = rendererInitialised;
		float loadCount = 0;

		getline(file, line); //Total number of items
		float maxLoad = stof(line.c_str());

		getline(file, line); //Number of items to parse via threading
		int threadCount = atoi(line.c_str());
		int mid = threadCount / 2;

		//Parsing lines that do not contain paramaters will be split into two threads
		//Thread0 will read from the start to the middle
		//Thread1 will read from the middle to the end at the same time
		int threadLine = 0;

		vector<vector<string>> tokensHolder0;
		vector<vector<string>> tokensHolder1;

		vector<string> lines1;
		vector<string> lines0;

		while (getline(file, line)) {
			std::istringstream iss(line);
		
			vector<string> tokens{ istream_iterator<string>{iss},
			istream_iterator<string>{} };

			if (tokens.at(0) == "params") {
				ReadParameters(tokens, line, &renderInit, renderLoadingScreen);
			}
			else{
				if (threadLine < mid) {
					tokensHolder0.push_back(tokens);
					lines0.push_back(line);
				}
				else {
					tokensHolder1.push_back(tokens);
					lines1.push_back(line);
				}
				threadLine++;
			}

			loadCount++;
			if (renderInit && renderLoadingScreen) {
				GRenderer->Find("Renderer")->RenderLoadingScreen(loadCount, maxLoad);
			}
		}

		auto readloop1 = GThreadPool->Find("ThreadPool")->SubmitJob([](
			DataBase& db, vector<vector<string>> tokenshold, vector<string> lines, int linenum) {
			db.ReadLoop(tokenshold, lines, linenum);
		}, std::ref(*this), tokensHolder0, lines0, mid);

		auto readloop2 = GThreadPool->Find("ThreadPool")->SubmitJob([](
			DataBase& db, vector<vector<string>> tokenshold, vector<string> lines, int linenum) {
			db.ReadLoop(tokenshold, lines, linenum);
		}, std::ref(*this), tokensHolder1, lines1, threadCount - mid);

		file.close();
	}

	size_t CurrentSize() const {
		//size_t size = sizeof(*this);
		size_t size = 0;
		size += GThreadPool->GetCurrentSize();
		size += PhysicsObjects->GetCurrentSize();
		size += OBJMeshes->GetCurrentSize();
		size += GRenderer->GetCurrentSize();
		size += GPhysicsEngine->GetCurrentSize();
		size += GWindow->GetCurrentSize();
		size += GCamera->GetCurrentSize();
		size += Players->GetCurrentSize();
		size += Gamepads->GetCurrentSize();
		size += GFSMManager->GetCurrentSize();
		size += GAudioManager->GetCurrentSize();

		return size;
	}

	size_t MaxSize() const {
		//size_t size = sizeof(*this);
		size_t size = 0;
		size += GThreadPool->GetMaxSize();
		size += PhysicsObjects->GetMaxSize();
		size += OBJMeshes->GetMaxSize();
		size += GRenderer->GetMaxSize();
		size += GPhysicsEngine->GetMaxSize();
		size += GWindow->GetMaxSize();
		size += GCamera->GetMaxSize();
		size += Players->GetMaxSize();
		size += Gamepads->GetMaxSize();
		size += GFSMManager->GetMaxSize();
		size += GAudioManager->GetMaxSize();

		return size;
	}
	ResourceManager<ThreadPool>*	GThreadPool;
	ResourceManager<Renderer>*		GRenderer;
	ResourceManager<PhysicsEngine>*	GPhysicsEngine;
	ResourceManager<Window>*		GWindow;
	ResourceManager<OBJMesh>*		OBJMeshes;
	ResourceManager<PhysicsObject>* PhysicsObjects;
	ResourceManager<Camera>*		GCamera;
	ResourceManager<Player>*		Players;
	ResourceManager<Gamepad>*		Gamepads;
	ResourceManager<FSMManager>*	GFSMManager;
	ResourceManager<Profiler>*		GProfiler;
	ResourceManager<InputManager>*	GInputManager;
	ResourceManager<AudioManager>*	GAudioManager;
private:
	void InitialiseFunctionMap()
	{
		/*
		So in a text file I can specify which resource
		manager to add the new resource to.
		*/
		addmap["GThreadPool"] = &DataBase::AddToGThreadPool;
		addmap["PhysicsObjects"] = &DataBase::AddToPhysicsObjects;
		addmap["OBJMeshes"] = &DataBase::AddToOBJMeshes;
		addmap["GWindow"] = &DataBase::AddToGWindow;
		addmap["GRenderer"] = &DataBase::AddToGRenderer;
		addmap["PhysicsEngine"] = &DataBase::AddToPhysicsEngine;
		addmap["GCamera"] = &DataBase::AddToGCamera;
		addmap["Players"] = &DataBase::AddToPlayers;
		addmap["Gamepads"] = &DataBase::AddToGamepads;
		addmap["GFSMManager"] = &DataBase::AddToGFSMManager;
		addmap["GProfiler"] = &DataBase::AddToGProfiler;
		addmap["GInputManager"] = &DataBase::AddToGInputManager;
	}

	void AddToGInputManager(string resourcename) {
		InputManager* inputManager = new InputManager(GThreadPool->Find("ThreadPool"));
		GInputManager->Load(resourcename, inputManager);
	}

	void AddToGProfiler(string params) {
		Profiler* profiler = new Profiler(this);
		profiler->ReadParams(params);
		GProfiler->Load(profiler);
	}

	void AddToPhysicsEngine(string params) {
		PhysicsEngine* physics = new PhysicsEngine();
		GPhysicsEngine->Load(params, physics);
	}

	void AddToGFSMManager(string params) {
		FSMManager* fsmm = new FSMManager(this);
		fsmm->ReadParams(params);
		GFSMManager->Load(fsmm);
	}

	void AddToGCamera(string params) {
		Camera* camera = new Camera();
		camera->ReadParams(params);
		GCamera->Load(camera);
	}

	//Will only support single digit IDs for current needs
	void AddToGamepads(string resourcename) {
		/*
		string name = resourceName;
		string snum = name.substr(name.size() - 1, name.size());
		int id = atoi(snum.c_str());*/
		Gamepad* gamepad = new Gamepad();
		gamepad->ReadParams(resourcename);
		Gamepads->Load(resourcename, gamepad);
	}

	void AddToOBJMeshes(string meshLocation) {
		OBJMesh* objm = new OBJMesh(meshLocation);
		OBJMeshes->Load(meshLocation, objm);
	}

	void AddToPhysicsObjects(string resourceName) {
		PhysicsObjects->Load(resourceName, new PhysicsObject());
	}

	void AddToPlayers(string resourceName) {
		Player* p = new Player();
		Players->Load(resourceName, p);
	}

	void AddToGThreadPool(string params) {
		std::istringstream iss(params);
		vector<string> tokens{ istream_iterator<string>{iss},
			istream_iterator<string>{} };

		string name = tokens.at(0);

		ThreadPool* threadPool = new ThreadPool();
		GThreadPool->Load(name, threadPool);
	}

	void AddToGWindow(string params) {
		std::istringstream iss(params);
		vector<string> tokens{ istream_iterator<string>{iss},
			istream_iterator<string>{} };

		string name = tokens.at(0);
		int resX = atoi(tokens.at(1).c_str());
		int resY = atoi(tokens.at(2).c_str());
		bool fullscreen = atoi(tokens.at(3).c_str()) != 0;

		Window* win = new Window(name, resX, resY, fullscreen);
		GWindow->Load(name, win);

		FSM* f = new FSM(win->vars, "../Data/GameLogic/Window.txt");
		FSMManager* fm = GFSMManager->Find("GFSMManager");
		fm->AddFSM(f);
	}

	//WILL NOT USE READ FUNCTIONS FOR NOW
	//DUE TO NO ACCESS TO DB
	void AddToGRenderer(string params) {
		std::istringstream iss(params);
		vector<string> tokens{ istream_iterator<string>{iss},
			istream_iterator<string>{} };
		string name = tokens.at(0);
		string winName = tokens.at(1);

		Renderer* rend = new Renderer(*(GWindow->Find(winName)));
		GRenderer->Load(name, rend);
	}

	void ReadLoop(vector<vector<string>> tokens, vector<string> line, int numLines) {
		for (int i = 0; i < numLines; i++) {
			ReadItem(tokens.at(i), line.at(i));
		}
	}

	void ReadParameters(vector<string> tokens, string line, bool* renderInit, bool renderLoadingScreen) {
		//Remove parameters and type
		string params = line;

		params.erase(params.find(tokens.at(0)), tokens.at(0).size());
		params.erase(params.find(tokens.at(1)), tokens.at(1).size());
		AddToResourceManager(tokens.at(1), params);

		if (tokens.at(1) == "GRenderer" && *renderInit == false && renderLoadingScreen) {
			*renderInit = true;
			GRenderer->Find("Renderer")->InitialiseLoadingScreen();
		}
	}

	void ReadItem(vector<string> tokens, string line) {
		int numOfItems = atoi(tokens.at(0).c_str());

		if (numOfItems == 1) {
			this->AddToResourceManager(tokens.at(1), tokens.at(2));
		}
		else {
			for (int i = 0; i < numOfItems; i++) {
				string name = tokens.at(2) + std::to_string(i);
				this->AddToResourceManager(tokens.at(1), name);
			}
		}

	}
};


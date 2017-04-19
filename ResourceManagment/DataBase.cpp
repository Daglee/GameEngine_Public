#include "DataBase.h"

//Just a style choice.
#define MAX_MEM_PER_TYPE 1000
#define MAX_MEM_PLAYERS 2000
#define MAX_MEM_OBJMESHES 2000
#define MAX_MEM_PHYSOBJ 3000

#define PHYS_OBJ_BINS 5
#define OBJMESH_BINS 5
#define SINGLE_BIN 1

DataBase::DataBase()
{
	//No runtime type identification needed! Woo!
	Players			= new ResourceManager<Player>		(false, true);
	Gamepads		= new ResourceManager<Gamepad>		(false, true);
	PhysicsObjects	= new ResourceManager<PhysicsObject>(false, true);
	OBJMeshes		= new ResourceManager<OBJMesh>		(false, true);
	GThreadPool		= new ResourceManager<ThreadPool>	(false, false);
	GRenderer		= new ResourceManager<Renderer>		(false, false);
	GPhysicsEngine	= new ResourceManager<PhysicsEngine>(false, false);
	GWindow			= new ResourceManager<Window>		(false, false);
	GCamera			= new ResourceManager<Camera>		(false, false);
	GFSMManager		= new ResourceManager<FSMManager>	(false, false);
	GProfiler		= new ResourceManager<Profiler>		(false, false);
	GInputManager	= new ResourceManager<InputManager>	(false, false);
	GAudioManager	= new ResourceManager<AudioManager>	(false, false);

	InitialiseFunctionMap();
}

DataBase::~DataBase()
{
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

void DataBase::InitialiseDataBase()
{
	PhysicsObjects	->Initialise("Physics Object Manager",	size_t(MAX_MEM_PHYSOBJ),	PHYS_OBJ_BINS);
	OBJMeshes		->Initialise("OBJMesh Manager",			size_t(MAX_MEM_OBJMESHES),	OBJMESH_BINS);
	Players			->Initialise("Players",					size_t(MAX_MEM_PLAYERS),	SINGLE_BIN);
	GThreadPool		->Initialise("GThreadPool",				size_t(MAX_MEM_PER_TYPE),	SINGLE_BIN);
	GRenderer		->Initialise("Renderer Manager",		size_t(MAX_MEM_PER_TYPE),	SINGLE_BIN);
	GPhysicsEngine	->Initialise("Physics Engine Manager",	size_t(MAX_MEM_PER_TYPE),	SINGLE_BIN);
	GFSMManager		->Initialise("GFSMManager",				size_t(MAX_MEM_PER_TYPE),	SINGLE_BIN);
	GProfiler		->Initialise("GProfiler",				size_t(MAX_MEM_PER_TYPE),	SINGLE_BIN);
	GInputManager	->Initialise("GInputManager",			size_t(MAX_MEM_PER_TYPE),	SINGLE_BIN);
	GAudioManager	->Initialise("GAudiomanager",			size_t(MAX_MEM_PER_TYPE),	SINGLE_BIN);

	GWindow			->Initialise("Window Manager",			sizeof(Window),			SINGLE_BIN);
	GCamera			->Initialise("Camera",					sizeof(Camera),			SINGLE_BIN);
	Gamepads		->Initialise("Gamepads",				sizeof(Gamepad) * 6,	SINGLE_BIN);

	GAudioManager	->Load("AudioManager", AudioManager::GetInstance());

	size_t databaseSize = MaxSize();
}

void DataBase::AddToResourceManager(string manager, string resourceName)
{
	FunctionMap::iterator iter = addmap.find(manager);
	if (iter != addmap.end()) {
		//Found
		(this->*(iter->second))(resourceName);
	}
}

void DataBase::StartUp(string filename, bool rendererInitialised, bool renderLoadingScreen)
{
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
		else {
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

size_t DataBase::CurrentSize() const
{
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

size_t DataBase::MaxSize() const
{
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

void DataBase::InitialiseFunctionMap()
{
	/*
	So in a text file I can specify which resource
	manager to add the new resource to.
	*/
	addmap["GThreadPool"]		= &DataBase::AddToGThreadPool;
	addmap["PhysicsObjects"]	= &DataBase::AddToPhysicsObjects;
	addmap["OBJMeshes"]			= &DataBase::AddToOBJMeshes;
	addmap["GWindow"]			= &DataBase::AddToGWindow;
	addmap["GRenderer"]			= &DataBase::AddToGRenderer;
	addmap["PhysicsEngine"]		= &DataBase::AddToPhysicsEngine;
	addmap["GCamera"]			= &DataBase::AddToGCamera;
	addmap["Players"]			= &DataBase::AddToPlayers;
	addmap["Gamepads"]			= &DataBase::AddToGamepads;
	addmap["GFSMManager"]		= &DataBase::AddToGFSMManager;
	addmap["GProfiler"]			= &DataBase::AddToGProfiler;
	addmap["GInputManager"]		= &DataBase::AddToGInputManager;
}

void DataBase::AddToGInputManager(string resourcename)
{
	InputManager* inputManager = new InputManager(GThreadPool->Find("ThreadPool"));
	GInputManager->Load(resourcename, inputManager);
}

void DataBase::AddToGProfiler(string params)
{
	Profiler* profiler = new Profiler(this);
	profiler->ReadParams(params);
	GProfiler->Load(profiler);
}

void DataBase::AddToPhysicsEngine(string params)
{
	PhysicsEngine* physics = new PhysicsEngine();
	GPhysicsEngine->Load(params, physics);
}

void DataBase::AddToGFSMManager(string params)
{
	FSMManager* fsmm = new FSMManager();
	fsmm->ReadParams(params);
	GFSMManager->Load(fsmm);
}

void DataBase::AddToGCamera(string params)
{
	Camera* camera = new Camera();
	camera->ReadParams(params);
	GCamera->Load(camera);
}

void DataBase::AddToGamepads(string resourcename)
{
	Gamepad* gamepad = new Gamepad();
	gamepad->ReadParams(resourcename);
	Gamepads->Load(resourcename, gamepad);
}

void DataBase::AddToOBJMeshes(string meshLocation)
{
	OBJMesh* objm = new OBJMesh(meshLocation);
	OBJMeshes->Load(meshLocation, objm);
}

void DataBase::AddToPhysicsObjects(string resourceName)
{
	PhysicsObjects->Load(resourceName, new PhysicsObject());
}

void DataBase::AddToPlayers(string resourceName)
{
	Player* p = new Player(this);
	Players->Load(resourceName, p);
}

void DataBase::AddToGThreadPool(string params)
{
	vector<string> tokens = Log::tokenise(params);

	string name = tokens.at(0);

	ThreadPool* threadPool = new ThreadPool();
	GThreadPool->Load(name, threadPool);
}

void DataBase::AddToGWindow(string params)
{
	vector<string> tokens = Log::tokenise(params);

	string name = tokens.at(0);
	int resX = atoi(tokens.at(1).c_str());
	int resY = atoi(tokens.at(2).c_str());
	bool fullscreen = atoi(tokens.at(3).c_str()) != 0;

	Window* win = new Window(this, name, resX, resY, fullscreen);
	GWindow->Load(name, win);
}

void DataBase::AddToGRenderer(string params)
{
	vector<string> tokens = Log::tokenise(params);

	string name = tokens.at(0);
	string winName = tokens.at(1);

	Renderer* rend = new Renderer(this, *(GWindow->Find(winName)));
	GRenderer->Load(name, rend);
}

void DataBase::ReadLoop(vector<vector<string>> tokens, vector<string> line, int numLines)
{
	for (int i = 0; i < numLines; i++) {
		ReadItem(tokens.at(i), line.at(i));
	}
}

void DataBase::ReadParameters(vector<string> tokens, string line, bool* renderInit, bool renderLoadingScreen)
{
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

void DataBase::ReadItem(vector<string> tokens, string line)
{
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
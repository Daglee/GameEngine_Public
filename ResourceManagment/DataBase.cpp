//#include "DataBase.h"
//
//DataBase::DataBase()
//{
//	//No runtime type identification needed!
//	GThreadPool = new ResourceManager<ThreadPool>(false, true);
//	PhysicsObjects = new ResourceManager<PhysicsObject>(false, true);
//	OBJMeshes = new ResourceManager<OBJMesh>(false, true);
//	GRenderer = new ResourceManager<Renderer>(false, true);
//	GPhysicsEngine = new ResourceManager<PhysicsEngine>(false, true);
//	GWindow = new ResourceManager<Window>(false, true);
//	GCamera = new ResourceManager<Camera>(false, true);
//	Players = new ResourceManager<Player>(false, true);
//	Gamepads = new ResourceManager<Gamepad>(false, true);
//	GFSMManager = new ResourceManager<FSMManager>(false, true);
//	GProfiler = new ResourceManager<Profiler>(false, true);
//	GInputManager = new ResourceManager<InputManager>(false, true);
//
//	InitialiseFunctionMap();
//}
//
//void DataBase::InitialiseFunctionMap()
//{	
//	/*
//	So in a text file I can specify which resource
//	manager to add the new resource to.
//	*/
//	addmap["GThreadPool"] = &DataBase::AddToGThreadPool;
//	addmap["PhysicsObjects"] = &DataBase::AddToPhysicsObjects;
//	addmap["OBJMeshes"] = &DataBase::AddToOBJMeshes;
//	addmap["GWindow"] = &DataBase::AddToGWindow;
//	addmap["GRenderer"] = &DataBase::AddToGRenderer;
//	addmap["PhysicsEngine"] = &DataBase::AddToPhysicsEngine;
//	addmap["GCamera"] = &DataBase::AddToGCamera;
//	addmap["Players"] = &DataBase::AddToPlayers;
//	addmap["Gamepads"] = &DataBase::AddToGamepads;
//	addmap["GFSMManager"] = &DataBase::AddToGFSMManager;
//	addmap["GProfiler"] = &DataBase::AddToGProfiler;
//	addmap["GInputManager"] = &DataBase::AddToGInputManager;
//}
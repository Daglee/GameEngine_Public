#pragma once

/////
//#include "ResourceManager.h"
//#include "../Profiler/MemoryWatcher.h"
//#include "../Threading/ThreadPool.h"
//#include "../Game/PhysicsObject.h"
//#include "../nclgl/Mesh.h"
//#include "../nclgl/OBJMesh.h"
//#include "../GameLogicFSM/FSMManager.h"
//#include "../GameLogicFSM/FSM.h"
//#include "../NCLGL/Renderer.h"
//#include "../nclgl/Gamepad.h"
//#include "../nclgl/Camera.h"
//#include "../Physics/PhysicsEngine.h"
//#include "../nclgl/Window.h"
//#include "../Game/Player.h"
//#include "../Profiler/Profiler.h"
//#include "../UISystem/InputManager.h"
//#include "../Game/AudioManager.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <thread>
#include <mutex>
#include <ctime>
/////

#include "Resource.h"
#include "Table.h"

#include <unordered_map>
#include <string>
#include <vector>

class DataBase
{
public:
	DataBase() {}
	~DataBase();

	void StartUp(std::string filename, bool rendererInitialised, bool renderLoadingScreen);

	void ReserveMemoryForAllTables();
	void AddResourceToTable(std::string tableName, std::string resourceName);

	void AddTable(std::string tableName, Table<Resource>* table)
	{
		tables[tableName] = table;
	}

	Table<Resource>* GetTable(std::string tableName)
	{
		return tables[tableName];
	}

	const size_t CurrentSize() const;
	const size_t MaxSize() const;

private:
	void ReadLoop(std::vector<std::vector<std::string>> tokens, std::vector<std::string> line, int numLines);

	void ReadItem(std::vector<std::string> tokens, std::string line);
	void ReadParameters(std::vector<std::string> tokens, std::string line,
		bool* renderInit, bool renderLoadingScreen);

	void ClearAllTables();

	std::unordered_map <std::string, Table<Resource>*> tables;
};


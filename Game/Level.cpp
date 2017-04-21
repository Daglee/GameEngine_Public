#include "Level.h"
#include "../ResourceManagment/DataBase.h"
#include "PhysicsObject.h"
#include "../nclgl/Vector4.h"
#include "../ResourceManagment/Log.h"
#include "../Game/AudioManager.h"

#include <sstream>
#include <iostream>
#include <fstream>

Level::Level(DataBase* db)
{
	database = db;
}

void Level::LoadAndInitialise(std::string directory) 
{
	//Directory file contains locations of the assets file and positions file
	std::ifstream file(directory);
	string path;

	getline(file, path);
	string assets		= path + "/assets.txt";
	string positions	= path + "/positions.txt";

	assetsfile = assets;
	LoadLevel(assets);
	InitialiseObjects(positions);

	/*
	  Read parameters for individual objects if files of each type exist.
	  More can be added.
	*/
	string physObj = path + "/PhysicsObjects.txt";
	if(Log::FileExists(physObj)) ReadPhysicsObject(physObj);

	string gamelogicFile = path + "/GameLogic.txt";
	if (Log::FileExists(gamelogicFile)) InitialiseGameLogic(gamelogicFile);

	string musicFile = path + "/Music.txt";
	if (Log::FileExists(musicFile)) InitialiseMusic(musicFile);
}

//Assets for database
void Level::LoadLevel(std::string filename) 
{
	std::ifstream file(filename);
	string line;

	getline(file, line);
	int maxLoad = atoi(line.c_str()); //Just for a second loading screen if one was added
	getline(file, line);

	while (getline(file, line)) {
		vector<string> tokens = Log::tokenise(line);

		int numOfItems = atoi(tokens.at(0).c_str());
		if (numOfItems == 1) {
			database->AddToResourceManager(tokens.at(1), tokens.at(2));
		}
		else {
			for (int i = 0; i < numOfItems; i++) {
				string name = tokens.at(2) + std::to_string(i);
				database->AddToResourceManager(tokens.at(1), name);
			}
		}
	}

	file.close();
}

//Read the assets file again and unload everything from the database
void Level::UnloadLevel() 
{
	std::ifstream file(assetsfile);
	string line;

	getline(file, line);
	int maxLoad = atoi(line.c_str());
	getline(file, line);

	while (getline(file, line)) {
		vector<string> tokens = Log::tokenise(line);

		int numOfItems = atoi(tokens.at(0).c_str());
		if (numOfItems == 1) {
			UnloadItem(tokens.at(1), tokens.at(2));
		}
		else {
			for (int i = 0; i < numOfItems; i++) {
				string name = tokens.at(2) + std::to_string(i);			
				UnloadItem(tokens.at(1), name);
			}
		}
	}

	file.close();
}

void Level::UnloadItem(std::string resourcemanager, std::string resourcename) 
{
	if (resourcemanager == "OBJMeshes") {
		database->OBJMeshes->Unload(resourcename);
	}
	else if (resourcemanager == "PhysicsObjects") {
		database->PhysicsObjects->Unload(resourcename);
	}
}

//Object positions + threading
void Level::InitialiseObjects(std::string filename) 
{
	std::ifstream file(filename);
	string line;

	getline(file, line);
	int threadCount = atoi(line.c_str());	//How many items do we need to read with threading?
	int mid = threadCount / 2;				//Where do we split the lines in two?

	int threadLine = 0;

	vector<string> lines1; //One thread for the first half of the file...
	vector<string> lines0; //And nother for the second half!

	while (getline(file, line)) {
		if (threadLine < mid) {
			lines0.push_back(line);
		}
		else {
			lines1.push_back(line);
		}
		threadLine++;
	}

	//1st half
	auto readloop1 = database->GThreadPool->Find("ThreadPool")->SubmitJob([](
		Level& lvl, vector<string> lines, int linenum) {
		lvl.ReadLoop(lines, linenum);
	}, std::ref(*this), lines0, mid);

	//2nd half
	auto readloop2 = database->GThreadPool->Find("ThreadPool")->SubmitJob([](
		Level& lvl, vector<string> lines, int linenum) {
		lvl.ReadLoop(lines, linenum);
	}, std::ref(*this), lines1, threadCount - mid);

	file.close(); 
}	

//To split reading in two
void Level::ReadLoop(std::vector<string> line, int numLines) 
{
	for (int i = 0; i < numLines; i++) {
		ReadObject(line.at(i));
	}
}

//Also set a couple of default values
void Level::ReadObject(std::string line) 
{
	vector<string> tokens = Log::tokenise(line);

	string manager = tokens.at(0);
	string name = tokens.at(1);
	float xPos = strtof(tokens.at(2).c_str(), 0);
	float yPos = strtof(tokens.at(3).c_str(), 0);
	float zPos = strtof(tokens.at(4).c_str(), 0);

	if (manager == "PhysicsObjects") {
		PhysicsObject* p = database->PhysicsObjects->Find(name);
		
		p->UpdatePhysics(database->GPhysicsEngine->Find("PhysicsEngine"));
		p->UpdateRenderer(*database->GRenderer->Find("Renderer"));
		
		bool isSphere = std::atoi(tokens.at(6).c_str());
		p->isSphere = isSphere;
		if (!isSphere) {
			//There must be an extra 3 tokens to define the plane here! e.g. "1 0 0" = Vector3(1,0,0)
			float x = stof(tokens.at(7));
			float y = stof(tokens.at(8));
			float z = stof(tokens.at(9));
			p->GetRigidBody()->collider = new PlaneCollider(Vector3(x, y, z));
			p->AddMesh(*database->OBJMeshes->Find(tokens.at(10)));
			p->SetPosition(Vector3(xPos, yPos, zPos));
		}
		else {
			p->GetRigidBody()->collider = new SphereCollider(1);
			p->AddMesh(*database->OBJMeshes->Find(tokens.at(7)));
			p->SetPosition(Vector3(xPos, yPos, zPos));
		}
	}
}

/*
  Use of if statements allows easier file IO 
  regarding setting parameters and, 
  most importantly, setting them in any order.
*/
void Level::ReadPhysicsObject(std::string filename)
{
	std::ifstream file(filename);
	string line;

	getline(file, line);

	int numItems = std::atoi(line.c_str());

	for (int i = 0; i < numItems; ++i) {
		getline(file, line); //Obj name
		PhysicsObject* obj = database->PhysicsObjects->Find(line);

		while (getline(file, line)) {
			vector<string> tokens = Log::tokenise(line);
			string variable = tokens.at(0);

			if (variable == "size") {
				float x = stof(tokens.at(1));
				float y = stof(tokens.at(2));
				float z = stof(tokens.at(3));

				Vector3 size(x, y, z);

				obj->SetSize(size);
			}
			else if (variable == "colour") {
				float r = stof(tokens.at(1));
				float g = stof(tokens.at(2));
				float b = stof(tokens.at(3));
				float a = stof(tokens.at(4));

				Vector4 col(r, g, b, a);

				obj->GetSceneNode()->SetColour(col);
			}
			else if (variable == "texture") {
				obj->SetTexture(*(tokens.at(1)).c_str());
			}
			else if (variable == "gravity") {
				float gravity = stof(tokens.at(1));
				obj->GetRigidBody()->gravity = gravity;
			}
			else if (variable == "mass") {
				float mass = stof(tokens.at(1));
				obj->GetRigidBody()->UpdateMass(mass);
			}
			else if (variable == "damping") {
				float damping = stof(tokens.at(1));
				obj->GetRigidBody()->drag = damping;
			}
			else if (variable == "isMoveable") {
				int isMoveable = atoi(tokens.at(1).c_str());
				obj->GetRigidBody()->isStatic = isMoveable;
			}
			else if (variable == "atRest") {
				int atRest = atoi(tokens.at(1).c_str());
				obj->GetRigidBody()->atRest = atRest;
			}
			else if (variable == "tag") {
				string tag = tokens.at(1);
				obj->GetRigidBody()->tag = tag;
			}
			else if (tokens.at(0) == "-") break;
		}

	}

}

void Level::InitialiseGameLogic(std::string filename)
{
	std::ifstream file(filename);
	string line;

	FSMManager* fsmManager = database->GFSMManager->Find("GFSMManager");

	while (getline(file, line))
	{
		vector<std::string> tokens = Log::tokenise(line);
		std::string FSMName = tokens.at(0);
		std::string FSMFile = tokens.at(1);

		//Construct the FSM
		fsmManager->LateBuildFSM(FSMName, FSMFile);
	}
}

void Level::InitialiseMusic(std::string filename)
{
	std::ifstream file(filename);
	string line;

	while (getline(file, line)) {
		//Song's filename = the current line.
		AudioManager::GetInstance()->AddBackgroundSound(line);
	}
}
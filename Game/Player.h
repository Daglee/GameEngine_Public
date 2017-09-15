
#define NOMINMAX
#pragma once
#define NOMINMAX

#include "../nclgl/Vector3.h"
#include "CharacterModel.h"
#include "../ResourceManagment/ResourceBase.h"
#include "Gun.h"
#include "AudioManager.h"
#include "../GameLogicFSM/FSMUnit.h"

class DataBase;
class InputMapper;
class PhysicsEngine;
class Renderer;
class RigidBody;

#include <random>
#include <map>
#define CHARCT_MODEL_LOC "../Data/CharacterModel/PlayerModel.txt"

/*
  Physics entity wrapper.
  Contains gameplay mechanics focused on the player.
*/
class Player : public ResourceBase, public FSMUnit
{
public:
	Player(DataBase* database, int id);
	~Player();

	void ApplyInputs();
	void Move(const Vector3& pos);

	void CheckHealth();
	void AddPoints();
	void CheckGunChange();
	void DisplayHUD();

	void CheckRagdollLimits();
	void Ragdoll();

	void SetInputMapper(InputMapper* im) 
	{
		input = im;
	}

	CharacterModel* GetPlayerModel()
	{
		return playerModel;
	}

	InputMapper* GetInputMapper() 
	{
		return input;
	}

	RigidBody* GetRigidBody()
	{
		return &rigidBody;
	}

	void UpdatePhysics(PhysicsEngine* p) 
	{
		physicsEngine = p;
		p->AddRigidBody(&rigidBody);
	}

	void UpdateRenderer(Renderer* r) 
	{
		renderer = r;
		r->AddSceneNode(rigidBody.parentMesh);
	}

	void AddSpawnPoint(Vector3 newPoint)
	{
		spawnPoints.push_back(newPoint);
	}

	void Update(const float& msec, const float& timer = 0);

	void Read(string resourcename);
	void ReadParams(string params);

	Vector3 rotation;
	Gun*	gun;
	int		teamID;
	string walkingSoundName;

	Mesh* playerModelMesh;
	std::vector<Vector3> spawnPoints;
protected:
	void Despawn();
	void Respawn(const Vector3& spawnPoint);

	CharacterModel* playerModel;
	InputMapper* input; //Updated in game loop
	RigidBody rigidBody;
	Matrix4 playerRotation;

	//Gun Stuff
	DataBase* database;
	Renderer* renderer;
	PhysicsEngine* physicsEngine;
	Mesh* gunMesh;

	std::vector<PhysicsObject*> bodies;

	int deadFrames = 0;
	int idNumber = -1;

	float	teamid = 0;
	float	health = 100;
	float	damage;
	float	colourr = 1;
	float	colourg = 0;
	float	colourb = 1;
	float	coloura = 1;
	float	timer = 0;
	float	collider = 0;
	float	killstreak = 0;

	bool lockInputs		= false;
	bool transmitting	= false;
};

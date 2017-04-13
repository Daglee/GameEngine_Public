#pragma once

#include "../nclgl/Vector3.h"
#include "CharacterModel.h"
#include "../ResourceManagment/ResourceBase.h"
#include "Gun.h"
#include "AudioManager.h"

class DataBase;
class InputMapper;
class PhysicsEngine;
class Renderer;
class RigidBody;


#include <map>
#define CHARCT_MODEL_LOC "../Data/CharacterModel/PlayerModel.txt"

/*
  Physics entity wrapper.
  Contains gameplay mechanics focused on the player.
*/
class Player : public ResourceBase
{
public:
	Player();
	~Player();

	void ApplyInputs();
	void Move(const Vector3& pos);

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
		p->AddRigidBody(&rigidBody);
	}

	void UpdateRenderer(Renderer* r) 
	{
		r->AddSceneNode(rigidBody.parentMesh);
	}

	void Update(const float& msec, const float& timer = 0);

	void Read(string resourcename);
	void ReadParams(string params);

	Vector3 rotation;
	Gun*	gun;
	int		teamID;
	string walkingSoundName;

	//For the finite state machine
	std::unordered_map<std::string, float*>* vars;
protected:
	CharacterModel* playerModel;
	InputMapper* input; //Updated in game loop
	RigidBody rigidBody;

	float	respawnTime;
	float	health;
	float	damage;
	float	colourr = 1;
	float	colourg = 0;
	float	colourb = 1;
	float	coloura = 1;
	float	timer = 0;
	float	collider = 0;
};


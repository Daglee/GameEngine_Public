
#define NOMINMAX
#pragma once
#define NOMINMAX

#include "../nclgl/Vector3.h"
#include "CharacterModel.h"
#include "../ResourceManagment/ResourceBase.h"
#include "Gun.h"
#include "AudioManager.h"
#include "../GameLogicFSM/FSMUnit.h"
#include "PlayerController.h"
#include "GunInput.h"
#include "LifeSpan.h"
#include "WeaponChange.h"
#include "PerspectiveHeadsUpDisplay.h"
#include "TextParagraph.h"
#include "PlayerRagdollSet.h"
#include "SpawnSystem.h"

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

	void SetPlayerController(PlayerController* im);

	CharacterModel* GetPlayerModel() const;
	PlayerController* GetPlayerController() const;
	RigidBody* GetRigidBody();
	const int GetIDNumber() const;

	void UpdatePhysics(PhysicsEngine* newPhysicsEngine);
	void UpdateRenderer(Renderer* newRenderer);

	void Update(const float& msec, const float& timer = 0);

	void Read(string resourcename);
	void ReadParams(string params);

	Vector3 rotation;
	Gun*	gun;
	int		teamID;
	string walkingSoundName;
	Mesh* playerModelMesh;
	GunInput* gunInput;
protected:
	void Despawn();
	void Respawn();

	void PrepareHUD();
	void UpdateHUD();
	void DisplayHUD();

	CharacterModel* playerModel;
	RigidBody rigidBody;
	Matrix4 playerRotation;

	DataBase* database;
	Renderer* renderer;
	PhysicsEngine* physicsEngine;
	Mesh* gunMesh;

	LifeSpan lifeSpan;
	PlayerController* controller;
	PlayerRagdollSet* ragdolls;
	WeaponChange weaponChanger;
	PerspectiveHeadsUpDisplay headsUpDisplay;
	TextParagraph HUDText;

	int idNumber = -1;
	float	teamid = 0;
	float	damage;
	float	timer = 0;
	float	collider = 0;
	float	killstreak = 0;
	Vector4 colour;

	bool inputsLocked = false;
	bool transmitting = false;
};


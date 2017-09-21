#pragma once

#include "../ResourceManagment/Resource.h"
#include "PhysicsObject.h"
#include <vector>

class DataBase;

/*
  Example class for a weapon for each player. Will undergo plenty of change
  before the second hand in.
  Just spawns physics objects and launches them in a direction.
*/
class Gun : public Resource
{
public:
	Gun(DataBase* db, Renderer* rend, PhysicsEngine* p, Mesh* m, float reloadSpeed = 2000.0f,
		int bulletsPerMag = 20, float bulletSpeed = 8.0f, float frate = 450.0f);

	virtual ~Gun() {}

	virtual bool Fire(const Vector3& from, const Vector3& rotation, const int& id) = 0;
	virtual void Reload() = 0;

	void Read(string resourcename);
	void ReadParams(string params);

	float fireDelay;

	std::string parent = "";

	DataBase*		database;
	Renderer*		rend;
	PhysicsEngine*	phys;
	Mesh*			bulletMesh;

	virtual void InitialiseBullet(PhysicsObject* b) = 0;

	double	lastShotTime;
	double	lastReloadTime;

	vector<PhysicsObject*> magazine;

	int		bulletsPerMag;
	int		bulletsFired;
	float	bulletSpeed;
	float	reloadSpeed;
	float	tempReloadSpeed = 0;
	bool reloading = false;
};


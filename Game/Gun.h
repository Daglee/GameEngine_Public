#pragma once

#include "../ResourceManagment/ResourceBase.h"
#include "PhysicsObject.h"
#include <vector>

class DataBase;

/*
  Example class for a weapon for each player. Will undergo plenty of change
  before the second hand in.
  Just spawns physics objects and launches them in a direction.
*/
class Gun : public ResourceBase
{
public:
	Gun(DataBase* db, Renderer* rend, PhysicsEngine* p, Mesh* m, float reloadSpeed = 2000.0f,
		int bulletsPerMag = 20, float bulletSpeed = 8.0f, float frate = 450.0f);

	virtual ~Gun() {}

	bool Fire(const Vector3& from, const Vector3& rotation, const int& id);
	void Reload();

	void Read(string resourcename);
	void ReadParams(string params);
protected:
	void InitialiseBullet(PhysicsObject* b);

	DataBase*		database;
	Renderer*		rend;
	PhysicsEngine*	phys;
	Mesh*			bulletMesh;

	double	lastShotTime;
	double	lastReloadTime;
	float	fireDelay;

	vector<PhysicsObject*> magazine;

	int		bulletsPerMag;
	int		bulletsFired;
	float	bulletSpeed;
	float	reloadSpeed;
	float	tempReloadSpeed = 0;
};


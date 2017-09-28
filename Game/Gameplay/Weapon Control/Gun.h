#pragma once

#include "../ResourceManagment/Resources/Resource.h"
#include "../../Structure/PhysicsObject.h"
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
	Gun(DataBase* db, Renderer* rend, PhysicsEngine* p, Mesh* m, const float reloadSpeed = 2000.0f,
		const int bulletsPerMag = 20, const float bulletSpeed = 8.0f, const float frate = 450.0f);

	virtual ~Gun() {}

	virtual bool Fire(const Vector3& from, const Vector3& rotation, const int& id) = 0;
	virtual void Reload() = 0;

	virtual void Read(const string resourcename) override;
	virtual void ReadParams(const string params) override;

	float fireDelay;

	string parent = "";

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

private:
	void CheckReloadSpeed(const float reloadSpeed);
	void CheckFireDelay(const float fireDelay);
};


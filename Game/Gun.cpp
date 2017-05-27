#include "Gun.h"

#include "../ResourceManagment/DataBase.h"
#include "../ResourceManagment/Log.h"
#include "../nclgl/Vector3.h"

#include <time.h>

Gun::Gun(DataBase* db, Renderer* r, PhysicsEngine* p, Mesh* m, float reloadSpeed, int bulletsPerMag,
	float bulletSpeed, float fireDelay) : ResourceBase() 
{
	rend = r;
	phys = p;

	database = db;
	this->bulletsPerMag		= bulletsPerMag;
	this->bulletSpeed		= bulletSpeed;
	this->bulletMesh		= m;

	if		(reloadSpeed < 1000)	Log::Error("Reload Speed is too low.");
	else	this->tempReloadSpeed	= reloadSpeed;

	if		(fireDelay < 150)	Log::Error("Fire Rate is too high.");
	else	this->fireDelay = fireDelay;

	bulletsFired = 0;
	lastShotTime = 0;

	this->SetResourceSize(sizeof(*this));
}

void Gun::Read(string resourcename) 
{
	this->SetResourceName(resourcename);
}

void Gun::ReadParams(string params) 
{
	Read(params);
}
#include "Gun.h"

#include "../ResourceManagment/DataBase.h"
#include "../ResourceManagment/Log.h"

Gun::Gun(DataBase* db, Renderer* r, PhysicsEngine* p, Mesh* m, const float reloadSpeed, const int bulletsPerMag,
	const float bulletSpeed, const float fireDelay) : Resource()
{
	rend = r;
	phys = p;

	database = db;
	this->bulletsPerMag = bulletsPerMag;
	this->bulletSpeed = bulletSpeed;
	this->bulletMesh = m;

	CheckReloadSpeed(reloadSpeed);
	CheckFireDelay(fireDelay);

	bulletsFired = 0;
	lastShotTime = 0;

	this->SetSizeInBytes(sizeof(*this));
}

void Gun::Read(const string resourcename)
{
	this->SetName(resourcename);
}

void Gun::ReadParams(const string params)
{
	Read(params);
}

void Gun::CheckReloadSpeed(const float reloadSpeed)
{
	if (reloadSpeed < 1000)
	{
		Log::Error("Reload Speed is too low.");
	}
	else
	{
		this->tempReloadSpeed = reloadSpeed;
	}
}

void Gun::CheckFireDelay(const float fireDelay)
{
	if (fireDelay < 150)
	{
		Log::Error("Fire Rate is too high.");
	}
	else
	{
		this->fireDelay = fireDelay;
	}
}

#pragma once

#include "Gun.h"

class RocketLauncher : public Gun
{
public:
	RocketLauncher(DataBase* db, Renderer* rend, PhysicsEngine* p, Mesh* m, float reloadSpeed = 5000.0f,
		int bulletsPerMag = 5, float bulletSpeed = 15.0f, float frate = 700.0f);
	~RocketLauncher() {}

	bool Fire(const Vector3& from, const Vector3& rotation, const int& id);
	void Reload();

protected:
	void InitialiseBullet(PhysicsObject* b);
};


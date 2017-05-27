#pragma once

#include "Gun.h"

class Pistol : public Gun
{
public:
	Pistol(DataBase* db, Renderer* rend, PhysicsEngine* p, Mesh* m, float reloadSpeed = 2000.0f,
		int bulletsPerMag = 10, float bulletSpeed = 8.0f, float frate = 450.0f);

	~Pistol() {}

	bool Fire(const Vector3& from, const Vector3& rotation, const int& id);
	void Reload();

protected:
	void InitialiseBullet(PhysicsObject* b);
};


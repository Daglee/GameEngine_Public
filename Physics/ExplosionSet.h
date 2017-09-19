#pragma once

#include "../nclgl/Vector3.h"

class Renderer;
class RigidBody;
class GameObject;
class DataBase;

#include <vector>

class ExplosionSet
{
public:
	ExplosionSet(Renderer* renderer = nullptr, DataBase* database = nullptr);
	~ExplosionSet();

	bool InRadiusOfExplosion(const RigidBody* explosion, const RigidBody* rigidBody);
	void ExplosionResponse(RigidBody* explosion, RigidBody* rb);

	void RenderAllExplosions();
	void InitialiseExplosion(const Vector3& Position);

private:

	void ExpandExplosions();
	void PrepareDeletionOfMaxSizeExplosions();
	void ClearDeleteBuffer();

	Renderer* renderer;
	DataBase* database;

	std::vector<GameObject*> explosions;
	std::vector<GameObject*> deleteBuffer;
};


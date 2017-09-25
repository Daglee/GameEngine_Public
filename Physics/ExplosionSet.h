#pragma once

#include "../nclgl/Vector3.h"

#include <vector>

class Renderer;
class RigidBody;
class GameObject;
class DataBase;

class ExplosionSet
{
public:
	ExplosionSet(Renderer* renderer = nullptr, DataBase* database = nullptr);
	~ExplosionSet();

	bool InRadiusOfExplosion(const RigidBody* explosion, const RigidBody* rigidBody) const;
	void ExplosionResponse(RigidBody* explosion, RigidBody* rb);

	void RenderAllExplosions();
	void InitialiseExplosion(const Vector3& Position);

private:

	void ExpandExplosions() const;
	void PrepareDeletionOfMaxSizeExplosions();
	void ClearDeleteBuffer();

	Renderer* renderer;
	DataBase* database;

	std::vector<GameObject*> explosions;
	std::vector<GameObject*> deleteBuffer;
};

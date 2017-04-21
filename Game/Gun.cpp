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

bool Gun::Fire(const Vector3& from, const Vector3& rotation, const int& id) 
{
	double now = clock();
	bool fired = false;
	/*
	  Rate of firing of new bullets is independent 
	  of input. Depends on fire rate.
	*/
	if (now - lastShotTime > fireDelay
		&& lastReloadTime + reloadSpeed < now) {

		fired = true;

		Vector3 pos				= from;
		Vector3 displacement	= from;
		
		pos.z += -rotation.z;
		pos.x += rotation.x;
		pos.y = 20; //Just set a default spawn height

		//Create the bullet (not the most efficient way to do it)
		//But serves the purpose of demoing inputs for now.
		string tag = "bullet" + to_string(id) + to_string(bulletsFired);

		PhysicsObject* b = new PhysicsObject(rend, phys, false, true);
		database->PhysicsObjects->Load(tag, b);
		InitialiseBullet(b);
		b->SetPosition(pos);
		b->GetRigidBody()->tag = tag;

		//To help with reloading
		magazine.push_back(b);
		this->SetResourceSize(sizeof(*this));

		displacement.y =20;
		Vector3 direction = pos - displacement;

		float length = direction.Length();
		Vector3 unit = direction / length;

		/*
		  Unit vector used so all bulets travel at same 
		  starting velocity regardless of input.
		*/
		b->ApplyForce(unit * bulletSpeed);

		if (bulletsFired == bulletsPerMag) Reload();

		++bulletsFired;
		lastShotTime = now;
	}

	return fired;
}

void Gun::Reload() 
{
	//Clear out the bullets that have been fired from the scene
	if (bulletsFired != 0) {
		for (int i = 0; i < bulletsFired; ++i) {
			phys->RemoveRigidBody(magazine.at(i)->GetRigidBody());
			rend->RemoveSceneNode(magazine.at(i)->GetSceneNode());
			database->PhysicsObjects->Unload(magazine.at(i)->GetRigidBody()->tag);
		}
	}

	for (int i = 0; i < bulletsFired; ++i) {
		delete magazine.at(i);
	}

	bulletsFired = 0;
	magazine.clear();
	lastReloadTime = clock();
	reloadSpeed = tempReloadSpeed;
	this->SetResourceSize(sizeof(*this));
}

/*
  ****** TEMPORARY ******
    Will be replaced...
  */
void Gun::InitialiseBullet(PhysicsObject* b) 
{
	b->AddMesh(*bulletMesh);
	b->GetRigidBody()->parentMesh = b->GetSceneNode();
	b->SetSize(Vector3(7, 7, 7));
	b->GetRigidBody()->collider = new SphereCollider(7);
	b->GetRigidBody()->drag = 0.999;
	b->GetRigidBody()->UpdateMass(10);
	b->GetRigidBody()->gravity = -0.0f;
	b->GetRigidBody()->atRest = false;
	b->GetRigidBody()->isStatic = false;
	b->GetSceneNode()->SetColour(Vector4(1, 0, 0, 1));
	b->GetRigidBody()->atRest = false;
}

void Gun::Read(string resourcename) 
{
	this->SetResourceName(resourcename);
}

void Gun::ReadParams(string params) 
{
	Read(params);
}
#include "WeaponChange.h"

#include "Gun.h"
#include "RocketLauncher.h"
#include "Pistol.h"

#include "../GameLogicFSM/MessageSystem.h"
#include "../ResourceManagment/DataBase.h"
#include "../nclgl/Renderer.h"
#include "../nclgl/Mesh.h"
#include "../Physics/PhysicsEngine.h"

const bool WeaponChange::RocketLauncherAvailable(const std::string& playerTag)
{
	return MessageSystem::GetInstance()->MessageTransmitting(Log::Hash(playerTag + "rocketlauncher"));
}

const bool WeaponChange::MachineGunAvailable(const std::string& playerTag)
{
	return MessageSystem::GetInstance()->MessageTransmitting(Log::Hash(playerTag + "machinegun"));
}

Gun* WeaponChange::CopyToRocketLauncher(Gun* currentWeapon)
{
	std::string playerTag = currentWeapon->parent;
	DataBase* database = currentWeapon->database;
	Renderer* renderer = currentWeapon->rend;
	PhysicsEngine* physicsEngine = currentWeapon->phys;
	Mesh* weaponMesh = currentWeapon->bulletMesh;

	delete currentWeapon;
	RocketLauncher* newweapon = new RocketLauncher(database, renderer, physicsEngine, weaponMesh);
	newweapon->parent = playerTag;

	newWeapon = playerTag + "hasrocketlauncher";
	newWeaponAnnouncementToStop = playerTag + "hasmachinegun";
	previousAnnouncement = playerTag + "rocketlauncher";

	return newweapon;
}

Gun* WeaponChange::CopyToMachineGun(Gun* currentWeapon)
{
	std::string playerTag = currentWeapon->parent;
	DataBase* database = currentWeapon->database;
	Renderer* renderer = currentWeapon->rend;
	PhysicsEngine* physicsEngine = currentWeapon->phys;
	Mesh* weaponMesh = currentWeapon->bulletMesh;

	delete currentWeapon;
	Pistol* newweapon = new Pistol(database, renderer, physicsEngine, weaponMesh);
	newweapon->fireDelay = 150;
	newweapon->parent = playerTag;

	newWeapon = playerTag + "hasmachinegun";
	newWeaponAnnouncementToStop = playerTag + "hasrocketlauncher";
	previousAnnouncement = playerTag + "machinegun";

	return newweapon;
}

void WeaponChange::AnnounceChange()
{
	MessageSystem::GetInstance()->Transmit(Log::Hash(newWeapon), true);
}

void WeaponChange::StopPreviousEvents()
{
	MessageSystem::GetInstance()->StopTransmitting(Log::Hash(newWeaponAnnouncementToStop));
	MessageSystem::GetInstance()->StopTransmitting(Log::Hash(previousAnnouncement));
}

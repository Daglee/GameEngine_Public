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

void WeaponChange::SetToRocketLauncher(Gun* currentWeapon)
{
	std::string playerTag = currentWeapon->parent;
	DataBase* database = currentWeapon->database;
	Renderer* renderer = currentWeapon->rend;
	PhysicsEngine* physicsEngine = currentWeapon->phys;
	Mesh* weaponMesh = currentWeapon->bulletMesh;

	delete currentWeapon;
	currentWeapon = new RocketLauncher(database, renderer, physicsEngine, weaponMesh);
	currentWeapon->parent = playerTag;

	newWeapon = playerTag + "hasrocketlauncher";
	newWeaponAnnouncementToStop = playerTag + "hasmachinegun";
	previousAnnouncement = playerTag + "rocketlauncher";
}

void WeaponChange::SetToMachineGun(Gun* currentWeapon)
{
	std::string playerTag = currentWeapon->parent;
	DataBase* database = currentWeapon->database;
	Renderer* renderer = currentWeapon->rend;
	PhysicsEngine* physicsEngine = currentWeapon->phys;
	Mesh* weaponMesh = currentWeapon->bulletMesh;

	delete currentWeapon;
	currentWeapon = new Pistol(database, renderer, physicsEngine, weaponMesh);
	currentWeapon->fireDelay = 150;
	currentWeapon->parent = playerTag;

	newWeapon = playerTag + "hasmachinegun";
	newWeaponAnnouncementToStop = playerTag + "hasrocketlauncher";
	previousAnnouncement = playerTag + "machinegun";
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

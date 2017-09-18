#pragma once

#include <string>

class Gun;

class WeaponChange
{
public:
	WeaponChange() {}
	~WeaponChange() {}

	const bool RocketLauncherAvailable(const std::string& playerTag);
	const bool MachineGunAvailable(const std::string& playerTag);

	Gun* CopyToRocketLauncher(Gun* currentWeapon);
	Gun* CopyToMachineGun(Gun* currentWeapon);

	void AnnounceChange();
	void StopPreviousEvents();

private:
	std::string previousAnnouncement;
	std::string newWeaponAnnouncementToStop;
	std::string newWeapon;
};


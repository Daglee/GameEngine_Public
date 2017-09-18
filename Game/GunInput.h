#pragma once

#include "../nclgl/Vector3.h"

class InputMapper;
class Gun;

struct WeaponData
{
	Vector3* firingPosition;
	int weaponID;

	WeaponData(Vector3* firingPosition, int weaponID)
	{
		this->firingPosition = firingPosition;
		this->weaponID = weaponID;
	}

	WeaponData()
	{
		this->firingPosition = nullptr;
		this->weaponID = -1;
	}
};

class GunInput
{
public:
	GunInput(InputMapper* input, WeaponData weaponData);
	GunInput(InputMapper* input, Gun* weapon, WeaponData weaponData);
	~GunInput() {}

	void ApplyInputs();

	void SetWeapon(Gun* newWeapon);

private:
	void FireWeapon();
	void ReloadWeapon();

	Gun* weapon;
	InputMapper* input;
	WeaponData weaponData;
};


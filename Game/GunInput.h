#pragma once

#include "../nclgl/Vector3.h"

class InputMapper;
class Gun;

struct WeaponData
{
	Vector3* firingPosition;
	int weaponID;

	WeaponData(Vector3* firingPosition, const int weaponID)
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
	GunInput(InputMapper* input, const WeaponData weaponData);
	GunInput(InputMapper* input, Gun* weapon, const WeaponData weaponData);
	~GunInput() {}

	void ApplyInputs() const;

	void SetWeapon(Gun* newWeapon);

private:
	void FireWeapon() const;
	void ReloadWeapon() const;

	Gun* weapon;
	InputMapper* input;
	WeaponData weaponData;
};


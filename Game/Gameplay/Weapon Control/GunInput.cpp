#include "GunInput.h"

#include "../UISystem/InputMapper.h"
#include "Gun.h"
#include "../../Audio/AudioManager.h"
#include "../GameLogicFSM/MessageSystem.h"

GunInput::GunInput(InputMapper* input, const WeaponData weaponData)
{
	this->input = input;
	this->weapon = nullptr;
	this->weaponData = weaponData;
}

GunInput::GunInput(InputMapper* input, Gun* weapon, const WeaponData weaponData)
{
	this->input = input;
	this->weapon = weapon;
	this->weaponData = weaponData;
}

void GunInput::ApplyInputs() const
{
	if (input->Fired())
	{
		FireWeapon();
	}

	if (input->Reload())
	{
		ReloadWeapon();
	}
}

void GunInput::FireWeapon() const
{
	if (weapon->Fire(*weaponData.firingPosition, input->rawRotation, weaponData.weaponID))
	{
		Sound* shoot = new Sound("../Data/Sounds/14615__man__canon.wav"); //AudioManager will delete this.
		AudioManager::GetInstance()->TemporaryPlay(shoot, SOUNDPRIORITY_MEDIUM);
	}
	MessageSystem::GetInstance()->TransmitMessage(Log::Hash("player_fired"));
}

void GunInput::ReloadWeapon() const
{
	weapon->Reload();
}

void GunInput::SetWeapon(Gun* newWeapon)
{
	this->weapon = newWeapon;
}

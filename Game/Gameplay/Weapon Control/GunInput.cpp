#include "GunInput.h"

#include "../UISystem/Input Managment/Input/InputMapper.h"
#include "Gun.h"
#include "../../Audio/AudioManager.h"
#include "../GameLogicFSM/Messaging/MessageSystem.h"
#include "../../../ResourceManagement/Utilities/StringUtility.h"

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
	MessageSystem::GetInstance()->TransmitMessage(StringUtility::Hash("player_fired"));
}

void GunInput::ReloadWeapon() const
{
	weapon->Reload();
}

void GunInput::SetWeapon(Gun* newWeapon)
{
	this->weapon = newWeapon;
}

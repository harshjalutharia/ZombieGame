
#pragma once

#include "ZEnums.generated.h"


UENUM()
enum class EWeaponState : uint8
{
	Idle,
    Equipping,
    Reloading,
    Firing
};


UENUM()
enum class EInventorySlot : uint8
{
	INVALID_SLOT,
    Weapon1,
    Weapon2
};
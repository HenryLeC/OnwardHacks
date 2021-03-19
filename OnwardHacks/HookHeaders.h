#pragma once
#include "pch.h"

// SetOutlineActive
typedef void(__fastcall* tSetOutlineActive)(void*, bool);
tSetOutlineActive oSetOutlineActive;

// SetOutlineColor
typedef void(__fastcall* tSetOutlineColor)(void*, Color);
tSetOutlineColor oSetOutlineColor;

// Fire Weapon
typedef void(__fastcall* tFireWeapon)(uintptr_t, uintptr_t, uintptr_t, void*);
tFireWeapon oFireWeapon;

// Set Current Ammo
typedef void(__fastcall* tSetCurrentAmmo)(void*, INT32);
tSetCurrentAmmo oSetCurrentAmmo;

// CodeManagerAwake
typedef void(__fastcall* tCodeManagerAwake)(uintptr_t);
tCodeManagerAwake oCodeManagerAwake;

// CheckNumbers
typedef void(__fastcall* tCheckNumbers)(uintptr_t);
tCheckNumbers oCheckNumbers;

// DoCodeCorrect
typedef void(__fastcall* tDoCodeCorrect)(uintptr_t);
tDoCodeCorrect oDoCodeCorrect;

// WarPlayerAwake
typedef void(__fastcall* tWarPlayerAwake)(uintptr_t);
tWarPlayerAwake oWarPlayerAwake;

// Set Invicincibity
typedef void(__fastcall* tSetManualInvincibility)(uintptr_t, bool);
tSetManualInvincibility oSetManualInvincibility;
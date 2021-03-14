#pragma once
#include "pch.h"

// SetOutlineActive
typedef void(__fastcall* tSetOutlineActive)(void*, bool);
tSetOutlineActive oSetOutlineActive;

// SetOutlineColor
typedef void(__fastcall* tSetOutlineColor)(void*, Color);
tSetOutlineColor oSetOutlineColor;

// Fire Weapon
typedef void(__fastcall* tFireWeapon)(uintptr_t, void*, void*, void*);
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
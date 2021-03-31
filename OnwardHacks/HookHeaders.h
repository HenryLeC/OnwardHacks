#pragma once
#include "pch.h"

// SetOutlineActive
typedef void(__fastcall* tSetOutlineActive)(uintptr_t, bool);
tSetOutlineActive oSetOutlineActive;

// SetOutlineColor
typedef void(__fastcall* tSetOutlineColor)(uintptr_t, uintptr_t);
tSetOutlineColor oSetOutlineColor;

// Fire Weapon
typedef void(__fastcall* tFireWeapon)(uintptr_t, uintptr_t, uintptr_t, void*);
tFireWeapon oFireWeapon;

// Set Current Ammo
typedef void(__fastcall* tSetCurrentAmmo)(uintptr_t, INT32);
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

// Get Player Faction
typedef int(__fastcall* tGetPlayerFaction)(uintptr_t);
tGetPlayerFaction oGetPlayerFaction;

// Set Invicincibity
typedef void(__fastcall* tSetManualInvincibility)(uintptr_t, bool);
tSetManualInvincibility oSetManualInvincibility;

// Get Faction Colors
typedef uintptr_t(__fastcall* tGetFactionColors)(uintptr_t, uintptr_t);
tGetFactionColors oGetFactionColors;

// RecalculatePoints
typedef bool(__fastcall* tRecalculatePoints)(uintptr_t);
tRecalculatePoints oRecalculatePoints;

// GetSteamId
typedef uintptr_t(__fastcall* tGetSteamID)(uintptr_t);
tGetSteamID oGetSteamID;

// Function Offsets
// SpectateOutline
extern uintptr_t SetOutlineActive_offset = 0x380030;
extern uintptr_t SetOutlineColor_offset = 0x3800C0;
// Weapon
extern uintptr_t FireWeapon_offset = 0x420220;
// CodeManager
extern uintptr_t CodeManagerAwake_offset = 0x12BBE20;
extern uintptr_t CheckNumbers_offset = 0x12BC0F0;
extern uintptr_t DoCodeCorrect_offset = 0x12BC4A0;
// Color Scheme
extern uintptr_t GetFactionColors_offset = 0x12BE030;
// War Player Script
extern uintptr_t GetPlayerFaction_offset = 0x12ADFD0;
// TentLoadoutEdit
extern uintptr_t RecalculatePoints_offset = 0x541E00;
// Get Steam ID
extern uintptr_t GetSteamID_offset = 0x1608830;
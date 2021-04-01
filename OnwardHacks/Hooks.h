#pragma once
#include <map>
#include "../OnwardHacksGUI//HacksSettings.h"
#include "Speedhack.h"
#include <fstream>

// SetOutlineActive
typedef void(__fastcall* tSetOutlineActive)(uintptr_t, bool);
extern tSetOutlineActive oSetOutlineActive;
extern uintptr_t SetOutlineActive_offset;

void __fastcall hkSetOutlineActive(uintptr_t pThis, bool active);

// SetOutlineColor
typedef void(__fastcall* tSetOutlineColor)(uintptr_t, uintptr_t);
extern tSetOutlineColor oSetOutlineColor;
extern uintptr_t SetOutlineColor_offset;

// Fire Weapon
typedef void(__fastcall* tFireWeapon)(uintptr_t, uintptr_t, uintptr_t, void*);
extern tFireWeapon oFireWeapon;
extern uintptr_t FireWeapon_offset;

void __fastcall hkFireWeapon(uintptr_t weapon, uintptr_t PlayerSource, uintptr_t forward, void* aiSourceId);

// Set Current Ammo
typedef void(__fastcall* tSetCurrentAmmo)(uintptr_t, INT32);
extern tSetCurrentAmmo oSetCurrentAmmo;

// CodeManagerAwake
typedef void(__fastcall* tCodeManagerAwake)(uintptr_t);
extern tCodeManagerAwake oCodeManagerAwake;
extern uintptr_t CodeManagerAwake_offset;

void __fastcall hkCodeManagerAwake(uintptr_t pThis);

// CheckNumbers
typedef void(__fastcall* tCheckNumbers)(uintptr_t);
extern tCheckNumbers oCheckNumbers;
extern uintptr_t CheckNumbers_offset;

void __fastcall hkCheckNumbers(uintptr_t pThis);

// DoCodeCorrect
typedef void(__fastcall* tDoCodeCorrect)(uintptr_t);
extern tDoCodeCorrect oDoCodeCorrect;
extern uintptr_t DoCodeCorrect_offset;
extern uintptr_t GetFactionColors_offset;

// WarPlayerAwake
typedef void(__fastcall* tWarPlayerAwake)(uintptr_t);
extern tWarPlayerAwake oWarPlayerAwake;

// Get Player Faction
typedef int(__fastcall* tGetPlayerFaction)(uintptr_t);
extern tGetPlayerFaction oGetPlayerFaction;
extern uintptr_t GetPlayerFaction_offset;

// Set Invicincibity
typedef void(__fastcall* tSetManualInvincibility)(uintptr_t, bool);
extern tSetManualInvincibility oSetManualInvincibility;

// Get Faction Colors
typedef uintptr_t(__fastcall* tGetFactionColors)(uintptr_t, uintptr_t);
extern tGetFactionColors oGetFactionColors;

uintptr_t __fastcall hkGetFactionColors(uintptr_t pThis, uintptr_t Faction);

// RecalculatePoints
typedef bool(__fastcall* tRecalculatePoints)(uintptr_t);
extern tRecalculatePoints oRecalculatePoints;
extern uintptr_t RecalculatePoints_offset;

bool __fastcall hkRecalculatePoints(uintptr_t pThis);

// GetSteamId
typedef uintptr_t(__fastcall* tGetSteamID)(uintptr_t);
extern tGetSteamID oGetSteamID;
extern uintptr_t GetSteamID_offset;

uintptr_t __fastcall hkGetSteamID(uintptr_t pThis);

// Steam ID Registry Edit
void EditRegKeys();

// idk wtf this is;
extern bool RegEditing;
extern uintptr_t colorScheme;

// Original Params
extern std::map<uintptr_t, float> defaultDamage;
extern std::map<uintptr_t, float> defaultRof;

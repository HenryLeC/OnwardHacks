#include <map>
#include "../OnwardHacksGUI//HacksSettings.h"
#include "Speedhack.h"
#include <fstream>
#include "Hooks.h"
#include <thread>

// globals
bool Running = true;

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
uintptr_t SetOutlineActive_offset = 0x380030;
uintptr_t SetOutlineColor_offset = 0x3800C0;
// Weapon
uintptr_t FireWeapon_offset = 0x420220;
// CodeManager
uintptr_t CodeManagerAwake_offset = 0x12BBE20;
uintptr_t CheckNumbers_offset = 0x12BC0F0;
uintptr_t DoCodeCorrect_offset = 0x12BC4A0;
// Color Scheme
uintptr_t GetFactionColors_offset = 0x12BE030;
// War Player Script
uintptr_t GetPlayerFaction_offset = 0x12ADFD0;
// TentLoadoutEdit
uintptr_t RecalculatePoints_offset = 0x541E00;
// Get Steam ID
uintptr_t GetSteamID_offset = 0x1608830;

// idk wtf this is;
uintptr_t colorScheme;

// Original Params
std::map<uintptr_t, float> defaultDamage = {};
std::map<uintptr_t, float> defaultRof = {};

// SetOutlineActive
void __fastcall hkSetOutlineActive(uintptr_t pThis, bool active) {
	if (ESP) {
		uintptr_t player = *(uintptr_t*)(pThis + 0x18);
		int faction = oGetPlayerFaction(player);
		if (colorScheme) {
			if (faction == 1) {
				oSetOutlineColor(pThis, colorScheme + 0x58);
			}
			else if (faction == 2) {
				oSetOutlineColor(pThis, colorScheme + 0x48);
			}
		}
		return oSetOutlineActive(pThis, true);
	}
	else {
		return oSetOutlineActive(pThis, active);
	}
}

// Fire Weapon
void __fastcall hkFireWeapon(uintptr_t weapon, uintptr_t PlayerSource, uintptr_t forward, void* aiSourceId) {
	//std::cout << "Weapon Object: " << std::hex << (weapon) << std::endl;
	// Infinite Ammo
	bool* infAmmo = (bool*)(weapon + 0xAC);
	// Rof
	float* rof = (float*)(weapon + 0x98);
	// No recoil
	bool* noRcoil = (bool*)(weapon + 0xC3);
	// Damage
	float* damage = (float*)(weapon + 0xF8);
	// 

	// Set Gun Params
	if (defaultDamage.find(weapon) == defaultDamage.end()) {
		defaultDamage[weapon] = *damage;
		defaultRof[weapon] = *rof;
	}

	// No Recoil
	if (NoRecoil) {
		*noRcoil = true;
	}
	else {
		*noRcoil = false;
	}

	// Infinite Ammo
	if (InfiniteAmmo) {
		*infAmmo = true;
	}
	else {
		*infAmmo = false;
	}

	// Damage
	if (bMaxDamage) {
		*damage = iMaxDamage;
	}
	else {
		*damage = defaultDamage[weapon];
	}

	// ROF
	if (bMaxRPM) {
		*rof = 60 / iMaxRPM;
	}
	else {
		*rof = defaultRof[weapon];
	}

	if (FastBurst) {
		for (int i = iFastBurst - 1; i <= 5; i++) {
			oFireWeapon(weapon, PlayerSource, forward, aiSourceId);
		}
	}
	return oFireWeapon(weapon, PlayerSource, forward, aiSourceId);
}

// Code manager Active
void __fastcall hkCodeManagerAwake(uintptr_t pThis) {
	//std::cout << pThis << std::endl;
	oCodeManagerAwake(pThis);
	// Auto Cap if no need for code and hack enabled
	if (AutoCap) {
		oDoCodeCorrect(pThis);
	}
}

// Check Code Numbers
void __fastcall hkCheckNumbers(uintptr_t pThis) {
	if (AnyCodeCap) {
		return oDoCodeCorrect(pThis);
	}
	else {
		return oCheckNumbers(pThis);
	}
}

// Get Faction Colors
uintptr_t __fastcall hkGetFactionColors(uintptr_t pThis, uintptr_t Faction) {
	colorScheme = pThis;
	return oGetFactionColors(pThis, Faction);
}

// Recalcuate Points
bool __fastcall hkRecalculatePoints(uintptr_t pThis) {
	oRecalculatePoints(pThis);
	return true;
}

// Get Steam ID
uintptr_t __fastcall hkGetSteamID(uintptr_t pThis) {
	INT64 SteamID = (INT64)oGetSteamID(pThis);
	/*INT64* id = (INT64*)(SteamID + 0x10);*/
	if (SteamIDSpoofer)
	{
		SteamID = 69694204206969420;
	}
	return SteamID;
}

// Listen for start command
void StartSpoofPatchListener() {
	while (Running) {
		if (SteamIDSpoofer) {
			std::ofstream StartFile("startSpoof");
			StartFile.close();
		}
		else {
			remove("startSpoof");
		}
	}
}

// Steam ID Registry Edit
void EditRegKeys() {
	// Remove Stop Key
	remove("startSpoof");
	remove("killRegPatch");

	// Registry Patch File
	std::ofstream RegFile("OnwardRegPatch.reg");
	RegFile << R""""(Windows Registry Editor Version 5.00

[HKEY_CURRENT_USER\SOFTWARE\Downpour Interactive\Onward]
"LastSteamWebAPIRequest_h1952494584"=hex:7b,22,52,65,73,70,6f,6e,73,65,22,3a,\
  7b,22,72,65,73,70,6f,6e,73,65,22,3a,7b,22,70,6c,61,79,65,72,73,22,3a,5b,7b,\
  22,73,74,65,61,6d,69,64,22,3a,22,36,39,36,39,34,32,30,34,32,30,36,39,36,39,\
  34,32,30,22,2c,22,63,6f,6d,6d,75,6e,69,74,79,76,69,73,69,62,6c,65,73,74,61,\
  74,65,22,3a,22,22,2c,22,70,72,6f,66,69,6c,65,73,74,61,74,65,22,3a,31,2c,22,\
  70,65,72,73,6f,6e,61,6e,61,6d,65,22,3a,22,e6,2d,af,22,2c,22,6c,61,73,74,6c,\
  6f,67,6f,66,66,22,3a,30,2c,22,70,72,6f,66,69,6c,65,75,72,6c,22,3a,22,68,74,\
  74,70,73,3a,2f,2f,73,74,65,61,6d,63,6f,6d,6d,75,6e,69,74,79,2e,63,6f,6d,2f,\
  70,72,6f,66,69,6c,65,73,2f,36,39,36,39,34,32,30,34,32,30,36,39,36,39,34,32,\
  30,2f,22,2c,22,61,76,61,74,61,72,22,3a,22,68,74,74,70,73,3a,2f,2f,73,74,65,\
  61,6d,63,64,6e,2d,61,2e,61,6b,61,6d,61,69,68,64,2e,6e,65,74,2f,73,74,65,61,\
  6d,63,6f,6d,6d,75,6e,69,74,79,2f,70,75,62,6c,69,63,2f,69,6d,61,67,65,73,2f,\
  61,76,61,74,61,72,73,2f,37,30,2f,36,39,36,39,34,32,30,34,32,30,36,39,36,39,\
  34,32,30,2e,6a,70,67,22,2c,22,61,76,61,74,61,72,6d,65,64,69,75,6d,22,3a,22,\
  68,74,74,70,73,3a,2f,2f,73,74,65,61,6d,63,64,6e,2d,61,2e,61,6b,61,6d,61,69,\
  68,64,2e,6e,65,74,2f,73,74,65,61,6d,63,6f,6d,6d,75,6e,69,74,79,2f,70,75,62,\
  6c,69,63,2f,69,6d,61,67,65,73,2f,61,76,61,74,61,72,73,2f,37,30,2f,36,39,36,\
  39,34,32,30,34,32,30,36,39,36,39,34,32,30,2e,6a,70,67,22,2c,22,61,76,61,74,\
  61,72,66,75,6c,6c,22,3a,22,68,74,74,70,73,3a,2f,2f,73,74,65,61,6d,63,64,6e,\
  2d,61,2e,61,6b,61,6d,61,69,68,64,2e,6e,65,74,2f,73,74,65,61,6d,63,6f,6d,6d,\
  75,6e,69,74,79,2f,70,75,62,6c,69,63,2f,69,6d,61,67,65,73,2f,61,76,61,74,61,\
  72,73,2f,37,30,2f,36,39,36,39,34,32,30,34,32,30,36,39,36,39,34,32,30,2e,6a,\
  70,67,22,2c,22,70,65,72,73,6f,6e,61,73,74,61,74,65,22,3a,31,7d,5d,7d,7d,7d


)"""";
	RegFile.close();

	// Batch File that loops the reg patch
	std::ofstream BatchFile("RunPatch.bat");
	BatchFile << R""""(
:loop
if not exist killRegPatch goto spoof_check
	exit
:spoof_check
if exist startSpoof goto process_it 
    timeout /t 5 /nobreak >nul
	goto loop
:process_it
REG IMPORT OnwardRegPatch.reg
goto loop
)"""";
	BatchFile.close();
	std::thread StartSpoofPatch(StartSpoofPatchListener);
	StartSpoofPatch.detach();
	system("RunPatch.bat");
	//system("RunPatch.bat");
}

void KillRegPatch() {
	std::ofstream KillRegFile("killRegPatch");
	KillRegFile.close();
}
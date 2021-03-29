// dllmain.cpp : Main Hooking Logic And Input Logic.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <map>
#include <unordered_map>
#include "Detours/detours.h"
#include "HookHeaders.h"
#include "Speedhack.h"

// Random
const std::string inputPre = ">>> ";
uintptr_t currentPlayer;
uintptr_t colorScheme;

// Hacks Settings
int shotsPerBurst = 6;
// Gun
float damageSetting = 2000;
float rofSetting = 0.00000001;

enum Hacks { ESP, NoRecoil, InfiniteAmmo, FastBurst, AutoCap, MaxHealth, MaxDamage, MaxROF, SpeedHack };
enum HackSettings {NeedCode};

std::unordered_map< Hacks, bool, std::hash<int> > enabledHacks = {
	{ESP, false},
	{NoRecoil, false},
	{InfiniteAmmo, false},
	{FastBurst, false},
	{AutoCap, false},
	{MaxHealth, false},
	{MaxDamage, false},
	{SpeedHack, false},
};
std::unordered_map<HackSettings, bool> hacksSettings = {
	{NeedCode, false}
};

// Original Params
std::map<uintptr_t, float> defaultDamage = {};
std::map<uintptr_t, float> defaultRof = {};

// SetOutlineActive
void __fastcall hkSetOutlineActive(uintptr_t pThis, bool active) {
	if (enabledHacks[ESP]) {
		uintptr_t player = *(uintptr_t*)(pThis + 0x18);
		int faction = oGetPlayerFaction(player);
		if (faction == 1) {
			oSetOutlineColor(pThis, colorScheme + 0x58);
		}
		else if(faction == 2) {
			oSetOutlineColor(pThis, colorScheme + 0x48);
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
	if (enabledHacks[NoRecoil]) {
		*noRcoil = true;
	}
	else {
		*noRcoil = false;
	}

	// Infinite Ammo
	if (enabledHacks[InfiniteAmmo]) {
		*infAmmo = true;
	}
	else {
		*infAmmo = false;
	}

	// Damage
	if (enabledHacks[MaxDamage]) {
		*damage = damageSetting;
	}
	else {
		*damage = defaultDamage[weapon];
	}

	// ROF
	if (enabledHacks[MaxROF]) {
		*rof = rofSetting;
	}
	else {
		*rof = defaultRof[weapon];
	}

	if (enabledHacks[FastBurst]) {
		for (int i = shotsPerBurst - 1; i <= 5; i++) {
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
	if (enabledHacks[AutoCap] && !hacksSettings[NeedCode]) {
		oDoCodeCorrect(pThis);
	}
}

// Check Code Numbers
void __fastcall hkCheckNumbers(uintptr_t pThis) {
	if (enabledHacks[AutoCap] && hacksSettings[NeedCode]) {
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

DWORD WINAPI HackThread(HMODULE hModule) {
	//Create Console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONIN$", "r", stdin);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	std::string key;

	/*std::cout << "Enter Product Key: ";
	std::cin >> key;*/

	bool running = true;
	
	system("cls"); // Clear Console
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	// Onward Hax Text
	std::cout << "                                                                                                                 \n    ,----..                                                                         ,--,                         \n   /   /   \\                                                                      ,--.\'|                         \n  /   .     :                                                  ,---,           ,--,  | :                         \n .   /   ;.  \\      ,---,         .---.             __  ,-.  ,---.\'|        ,---.\'|  : \'                         \n.   ;   /  ` ;  ,-+-. /  |       /. ./|           ,\' ,\'/ /|  |   | :        |   | : _\' |             ,--,  ,--,  \n;   |  ; \\ ; | ,--.\'|\'   |    .-\'-. \' |  ,--.--.  \'  | |\' |  |   | |        :   : |.\'  |  ,--.--.    |\'. \\/ .`|  \n|   :  | ; | \'|   |  ,\"\' |   /___/ \\: | /       \\ |  |   ,\',--.__| |        |   \' \'  ; : /       \\   \'  \\/  / ;  \n.   |  \' \' \' :|   | /  | |.-\'.. \'   \' ..--.  .-. |\'  :  / /   ,\'   |        \'   |  .\'. |.--.  .-. |   \\  \\.\' /   \n\'   ;  \\; /  ||   | |  | /___/ \\:     \' \\__\\/: . .|  | \' .   \'  /  |        |   | :  | \' \\__\\/: . .    \\  ;  ;   \n \\   \\  \',  / |   | |  |/.   \\  \' .\\    ,\" .--.; |;  : | \'   ; |:  |        \'   : |  : ; ,\" .--.; |   / \\  \\  \\  \n  ;   :    /  |   | |--\'  \\   \\   \' \\ |/  /  ,.  ||  , ; |   | \'/  \'        |   | \'  ,/ /  /  ,.  | ./__;   ;  \\ \n   \\   \\ .\'   |   |/       \\   \\  |--\";  :   .\'   \\---\'  |   :    :|        ;   : ;--\' ;  :   .\'   \\|   :/\\  \\ ; \n    `---`     \'---\'         \\   \\ |   |  ,     .-./       \\   \\  /          |   ,/     |  ,     .-./`---\'  `--`  \n                             \'---\"     `--`---\'            `----\'           \'---\'       `--`---\'                 \n                                                                                                                 \n" << std::endl;
	std::cout << "Injecting..." << std::endl;

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
#if SILENT
	std::cout << "[1] ESP \n[2] No Recoil \n[3] Infinite Ammo \n[4] Fast Burst \n[5] Auto-Cap \n[6] Maxed Damage \n[7] Max Rate Of Fire\n[8] Speedhack\n[-1] Disinject Hacks\n[0] Help \n";
#endif
#if BLATANT
	std::cout << "[1] ESP \n[2] No Recoil \n[3] Infinite Ammo \n[4] Fast Burst (Disabled) \n[5] Any Code Capper \n[6] Maxed Damage (Disabled) \n[7] Max Rate Of Fire (Disabled) \n[8] Speedhack (Disabled) \n[-1] Disinject Hacks\n[0] Help \n";
#endif
	std::cout << inputPre;
	

	while (running)
	{
		int input;
		// Get Input
		std::cin >> input;
		switch (input) {
		case 1:
			enabledHacks[ESP] = !enabledHacks[ESP];
			std::cout << "ESP enabled: " << enabledHacks[ESP] << std::endl;
			break;
		case 2:
			enabledHacks[NoRecoil] = !enabledHacks[NoRecoil];
			std::cout << "No Recoil enabled: " << enabledHacks[NoRecoil] << std::endl;
			break;
		case 3:
			enabledHacks[InfiniteAmmo] = !enabledHacks[InfiniteAmmo];
			std::cout << "Infinite Ammo enabled: " << enabledHacks[InfiniteAmmo] << std::endl;
			break;
		case 4:
#if SILENT
			enabledHacks[FastBurst] = !enabledHacks[FastBurst];
			std::cout << "Rounds Per Burst: ";
			std::cin >> shotsPerBurst;
			std::cout << "Fast Burst enabled: " << enabledHacks[FastBurst] << std::endl;
#endif
#if BLATANT
			std::cout << "Fast Burst is Disabled in this package" << std::endl;
#endif
			break;
		case 5:
			enabledHacks[AutoCap] = !enabledHacks[AutoCap];
#if SILENT
			if (enabledHacks[AutoCap]) {
				std::cout << "Type 0 to not require typing in code, or 1 to require a code: ";
				std::cin >> hacksSettings[NeedCode];
			}
#endif
#if BLATANT
			hacksSettings[NeedCode] = true;
#endif
			std::cout << "Auto Cap enabled: " << enabledHacks[AutoCap] << std::endl;
			break;
		case 6:
#if SILENT
			enabledHacks[MaxDamage] = !enabledHacks[MaxDamage];
			std::cout << "Enter Damage per round (2000) for instant death: ";
			std::cin >> damageSetting;
			std::cout << "Max Damage enabled: " << enabledHacks[MaxDamage] << std::endl;
#endif
#if BLATANT
			std::cout << "Max Damage is Disabled in this package" << std::endl;
#endif
			break;
		case 7:
#if SILENT
			enabledHacks[MaxROF] = !enabledHacks[MaxROF];
			std::cout << "Enter Rate of fire in rpm: ";
			int rof;
			std::cin >> rof;
			rofSetting = 60 / rof;

			std::cout << "Max Rate Of Fire enabled: " << enabledHacks[MaxROF] << std::endl;
#endif
#if BLATANT
			std::cout << "Max Rate of Fire is Disabled in this package" << std::endl;
#endif
			break;
		case 8:
#if SILENT
			enabledHacks[SpeedHack] = !enabledHacks[SpeedHack];
			if (enabledHacks[SpeedHack]) {
				std::cout << "Enter Speed Multiplier: ";
				float SpeedMul;
				std::cin >> SpeedMul;
				Speedhack::SetSpeed(SpeedMul);
			}
			else {
				Speedhack::SetSpeed(1.0);
			}
			std::cout << "Speed Hack enabled: " << enabledHacks[SpeedHack] << std::endl;
#endif
#if BLATANT
			std::cout << "Speed Hack is Disabled in this package" << std::endl;
#endif
			break;
		case -1:
			running = false;
			break;
		case 0:
			std::cout << "Type the number cooresponding to the option you would like and press enter" << std::endl;
			break;
		}
		std::cout << inputPre;
		////Keypress listening
		//if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
		//	espEnabled = !espEnabled;
		//	std::cout << "ESP enabled: " << espEnabled << std::endl;
		//}
		//else if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
		//	multiShotEnabled = !multiShotEnabled;
		//	std::cout << "Multi Shot enabled: " << multiShotEnabled << std::endl;
		//	if (multiShotEnabled) {
		//		std::cout << "Enter Amount of Bullets Per Burst: ";
		//		std::cin >> shotsPerBurst;
		//		std::cout << "Shots Per Burst: " << shotsPerBurst << std::endl;
		//	}
		//}
		//else if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
		//	gunHacksEnabled = !gunHacksEnabled;
		//	std::cout << "Gun Hacks enabled: " << gunHacksEnabled << std::endl;
		//}
		//else if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
		//	break;
		//}
	}

	if (f) {
		fclose(f);
		fclose(stdout);
		fclose(stdin);
	}
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	{
		HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0);
		if (hThread != nullptr)
			CloseHandle(hThread);

		// Detour setup
		// Offsets
		uintptr_t assemblyAddress = (uintptr_t)GetModuleHandleW(L"GameAssembly.dll");

		oSetOutlineActive = (tSetOutlineActive)(assemblyAddress + SetOutlineActive_offset);

		oFireWeapon = (tFireWeapon)(assemblyAddress + FireWeapon_offset);

		oSetOutlineColor = (tSetOutlineColor)(assemblyAddress + SetOutlineColor_offset);

		oCodeManagerAwake = (tCodeManagerAwake)(assemblyAddress + CodeManagerAwake_offset);

		oCheckNumbers = (tCheckNumbers)(assemblyAddress + CheckNumbers_offset);

		oDoCodeCorrect = (tDoCodeCorrect)(assemblyAddress + DoCodeCorrect_offset);

		oGetFactionColors = (tGetFactionColors)(assemblyAddress + GetFactionColors_offset);

		oGetPlayerFaction = (tGetPlayerFaction)(assemblyAddress + GetPlayerFaction_offset);

		oRecalculatePoints = (tRecalculatePoints)(assemblyAddress + RecalculatePoints_offset);

		// Attach Detours
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)oSetOutlineActive, hkSetOutlineActive);
		DetourAttach(&(PVOID&)oFireWeapon, hkFireWeapon);
		DetourAttach(&(PVOID&)oCodeManagerAwake, hkCodeManagerAwake);
		DetourAttach(&(PVOID&)oCheckNumbers, hkCheckNumbers);
		DetourAttach(&(PVOID&)oGetFactionColors, hkGetFactionColors);
		DetourAttach(&(PVOID&)oRecalculatePoints, hkRecalculatePoints);

		LONG lError = DetourTransactionCommit();

		// Speedhack
		Speedhack::Setup();
		Speedhack::SetSpeed(1.0);

		if (lError != NO_ERROR) {
			MessageBox(HWND_DESKTOP, L"Failed to detour", L"timb3r", MB_OK);
			return FALSE;
		}
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		// Detour Teardown
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)oSetOutlineActive, hkSetOutlineActive);
		DetourDetach(&(PVOID&)oFireWeapon, hkFireWeapon);
		DetourDetach(&(PVOID&)oCodeManagerAwake, hkCodeManagerAwake);
		DetourDetach(&(PVOID&)oCheckNumbers, hkCheckNumbers);
		DetourDetach(&(PVOID&)oGetFactionColors, hkGetFactionColors);
		DetourDetach(&(PVOID&)oRecalculatePoints, hkRecalculatePoints);

		LONG lError = DetourTransactionCommit();

		// Speedhack
		Speedhack::Detach();


		if (lError != NO_ERROR) {
			MessageBox(HWND_DESKTOP, L"Failed to detour", L"timb3r", MB_OK);
		}
		break;
	}
	}

	return true;
}

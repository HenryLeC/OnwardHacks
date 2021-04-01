// dllmain.cpp : Main Hooking Logic And Input Logic.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <map>
#include <thread>
#include <fstream>
#include <unordered_map>
#include "Detours/detours.h"
#include "HookHeaders.h"
#include "Speedhack.h"
#include "../OnwardHacksGUI/gui.h"
#include "../OnwardHacksGUI/HacksSettings.h"

// idk wtf this is;
bool RegEditing = true;
uintptr_t colorScheme;

// Original Params
std::map<uintptr_t, float> defaultDamage = {};
std::map<uintptr_t, float> defaultRof = {};

// SetOutlineActive
void __fastcall hkSetOutlineActive(uintptr_t pThis, bool active) {
	if (ESP) {
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
		*rof = iMaxRPM;
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
	if (AutoCap && AnyCodeCap) {
		oDoCodeCorrect(pThis);
	}
}

// Check Code Numbers
void __fastcall hkCheckNumbers(uintptr_t pThis) {
	if (AutoCap && !AnyCodeCap) {
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
	SteamID = 69694204206969420;
	return SteamID;
}

// Steam ID Registry Edit
void EditRegKeys() {
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
	while (RegEditing) {
		//CreateProcessA("REG IMPORT OnwardRegPatch.reg >nul 2>nul", SW_HIDE);
	}
}

DWORD WINAPI HackThread(HMODULE hModule) {
	//Create Console
	/*AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONIN$", "r", stdin);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	std::string key;*/

	std::thread RegPatch(EditRegKeys);
	RegPatch.detach();

	/*std::cout << "Enter Product Key: ";
	std::cin >> key;*/

	bool running = true;
	
	//system("cls"); // Clear Console
	//SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	// Onward Hax Text
	/*std::cout << "                                                                                                                 \n    ,----..                                                                         ,--,                         \n   /   /   \\                                                                      ,--.\'|                         \n  /   .     :                                                  ,---,           ,--,  | :                         \n .   /   ;.  \\      ,---,         .---.             __  ,-.  ,---.\'|        ,---.\'|  : \'                         \n.   ;   /  ` ;  ,-+-. /  |       /. ./|           ,\' ,\'/ /|  |   | :        |   | : _\' |             ,--,  ,--,  \n;   |  ; \\ ; | ,--.\'|\'   |    .-\'-. \' |  ,--.--.  \'  | |\' |  |   | |        :   : |.\'  |  ,--.--.    |\'. \\/ .`|  \n|   :  | ; | \'|   |  ,\"\' |   /___/ \\: | /       \\ |  |   ,\',--.__| |        |   \' \'  ; : /       \\   \'  \\/  / ;  \n.   |  \' \' \' :|   | /  | |.-\'.. \'   \' ..--.  .-. |\'  :  / /   ,\'   |        \'   |  .\'. |.--.  .-. |   \\  \\.\' /   \n\'   ;  \\; /  ||   | |  | /___/ \\:     \' \\__\\/: . .|  | \' .   \'  /  |        |   | :  | \' \\__\\/: . .    \\  ;  ;   \n \\   \\  \',  / |   | |  |/.   \\  \' .\\    ,\" .--.; |;  : | \'   ; |:  |        \'   : |  : ; ,\" .--.; |   / \\  \\  \\  \n  ;   :    /  |   | |--\'  \\   \\   \' \\ |/  /  ,.  ||  , ; |   | \'/  \'        |   | \'  ,/ /  /  ,.  | ./__;   ;  \\ \n   \\   \\ .\'   |   |/       \\   \\  |--\";  :   .\'   \\---\'  |   :    :|        ;   : ;--\' ;  :   .\'   \\|   :/\\  \\ ; \n    `---`     \'---\'         \\   \\ |   |  ,     .-./       \\   \\  /          |   ,/     |  ,     .-./`---\'  `--`  \n                             \'---\"     `--`---\'            `----\'           \'---\'       `--`---\'                 \n                                                                                                                 \n" << std::endl;
	std::cout << "Injecting..." << std::endl;*/

	mainGUI();

//	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
//#if SILENT
//	std::cout << "[1] ESP \n[2] No Recoil \n[3] Infinite Ammo \n[4] Fast Burst \n[5] Auto-Cap \n[6] Maxed Damage \n[7] Max Rate Of Fire\n[8] Speedhack\n[-1] Disinject Hacks\n[0] Help \n";
//#endif
//#if BLATANT
//	std::cout << "[1] ESP \n[2] No Recoil \n[3] Infinite Ammo \n[4] Fast Burst (Disabled) \n[5] Any Code Capper \n[6] Maxed Damage (Disabled) \n[7] Max Rate Of Fire \n[8] Speedhack \n[-1] Disinject Hacks\n[0] Help \n";
//#endif
//	std::cout << inputPre;
//	
//
//	while (running)
//	{
//		int input;
//		// Get Input
//		std::cin >> input;
//		switch (input) {
//		case 1:
//			enabledHacks[ESP] = !enabledHacks[ESP];
//			std::cout << "ESP enabled: " << enabledHacks[ESP] << std::endl;
//			break;
//		case 2:
//			enabledHacks[NoRecoil] = !enabledHacks[NoRecoil];
//			std::cout << "No Recoil enabled: " << enabledHacks[NoRecoil] << std::endl;
//			break;
//		case 3:
//			enabledHacks[InfiniteAmmo] = !enabledHacks[InfiniteAmmo];
//			std::cout << "Infinite Ammo enabled: " << enabledHacks[InfiniteAmmo] << std::endl;
//			break;
//		case 4:
//#if SILENT
//			enabledHacks[FastBurst] = !enabledHacks[FastBurst];
//			std::cout << "Rounds Per Burst: ";
//			std::cin >> shotsPerBurst;
//			std::cout << "Fast Burst enabled: " << enabledHacks[FastBurst] << std::endl;
//#endif
//#if BLATANT
//			std::cout << "Fast Burst is Disabled in this package" << std::endl;
//#endif
//			break;
//		case 5:
//			enabledHacks[AutoCap] = !enabledHacks[AutoCap];
//#if SILENT
//			if (enabledHacks[AutoCap]) {
//				std::cout << "Type 0 to not require typing in code, or 1 to require a code: ";
//				std::cin >> hacksSettings[NeedCode];
//			}
//#endif
//#if BLATANT
//			hacksSettings[NeedCode] = true;
//#endif
//			std::cout << "Auto Cap enabled: " << enabledHacks[AutoCap] << std::endl;
//			break;
//		case 6:
//#if SILENT
//			enabledHacks[MaxDamage] = !enabledHacks[MaxDamage];
//			std::cout << "Enter Damage per round (2000) for instant death: ";
//			std::cin >> damageSetting;
//			std::cout << "Max Damage enabled: " << enabledHacks[MaxDamage] << std::endl;
//#endif
//#if BLATANT
//			std::cout << "Max Damage is Disabled in this package" << std::endl;
//#endif
//			break;
//		case 7:
//			enabledHacks[MaxROF] = !enabledHacks[MaxROF];
//			std::cout << "Enter Rate of fire in rpm: ";
//			int rof;
//			std::cin >> rof;
//			rofSetting = 60 / rof;
//
//			std::cout << "Max Rate Of Fire enabled: " << enabledHacks[MaxROF] << std::endl;
//			break;
//		case 8:
//			enabledHacks[SpeedHack] = !enabledHacks[SpeedHack];
//			if (enabledHacks[SpeedHack]) {
//				std::cout << "Enter Speed Multiplier: ";
//				float SpeedMul;
//				std::cin >> SpeedMul;
//				Speedhack::SetSpeed(SpeedMul);
//			}
//			else {
//				Speedhack::SetSpeed(1.0);
//			}
//			std::cout << "Speed Hack enabled: " << enabledHacks[SpeedHack] << std::endl;
//			break;
//		case -1:
//			RegEditing = false;
//			running = false;
//			break;
//		case 0:
//			std::cout << "Type the number cooresponding to the option you would like and press enter" << std::endl;
//			break;
//		}
//		std::cout << inputPre;
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
//	}

	// Disinject Procedure
	RegEditing = false;

	/*if (f) {
		fclose(f);
		fclose(stdout);
		fclose(stdin);
	}
	FreeConsole();*/
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

		oGetSteamID = (tGetSteamID)(assemblyAddress + GetSteamID_offset);

		// Attach Detours
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)oSetOutlineActive, hkSetOutlineActive);
		DetourAttach(&(PVOID&)oFireWeapon, hkFireWeapon);
		DetourAttach(&(PVOID&)oCodeManagerAwake, hkCodeManagerAwake);
		DetourAttach(&(PVOID&)oCheckNumbers, hkCheckNumbers);
		DetourAttach(&(PVOID&)oGetFactionColors, hkGetFactionColors);
		DetourAttach(&(PVOID&)oRecalculatePoints, hkRecalculatePoints);
		DetourAttach(&(PVOID&)oGetSteamID, hkGetSteamID);

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
		DetourDetach(&(PVOID&)oGetSteamID, hkGetSteamID);

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

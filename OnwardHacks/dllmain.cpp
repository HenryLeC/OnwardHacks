// dllmain.cpp : Main Hooking Logic And Input Logic.
#include "pch.h"
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <map>
#include <stdio.h>
#include <vector>
#include "Detours/detours.h"
#include "Structs.h"
#include "HookHeaders.h"
#include "mem.h"
#include <io.h>
#include <fcntl.h>
#include <unordered_map>
#include "Security.h"
#include "Speedhack.h"

// Random
const std::string inputPre = ">>> ";
uintptr_t currentPlayer;

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
void __fastcall hkSetOutlineActive(void* pThis, bool active) {
	//std::cout << pThis << std::endl;
	if (enabledHacks[ESP]) {
		return oSetOutlineActive(pThis, true);
	}
	else {
		return oSetOutlineActive(pThis, active);
	}
}

// SetOutlineColor
void __fastcall hkSetOutlineColor(void* pThis, Color color) {
	std::cout << color.r << " " << color.g << " " << color.b << " " << color.a << " " << std::endl;
	oSetOutlineColor(pThis, color);
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

// War Player Awake Logic
void __fastcall hkWarPlayerAwake(uintptr_t pThis) {
	//currentPlayer = pThis + 0x10;
	//oSetManualInvincibility(pThis, true);
	oWarPlayerAwake(pThis);
	//std::cout << "Player: " << std::hex << pThis << std::dec << std::endl;
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
	std::cout << "[1] ESP \n[2] No Recoil \n[3] Infinite Ammo \n[4] Fast Burst \n[5] Auto-Cap \n[6] Maxed Damage \n[7] Max Rate Of Fire\n[8] Speedhack\n[-1] Disinject Hacks\n[0] Help \n";
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
			enabledHacks[FastBurst] = !enabledHacks[FastBurst];
			std::cout << "Rounds Per Burst: ";
			std::cin >> shotsPerBurst;
			std::cout << "Fast Burst enabled: " << enabledHacks[FastBurst] << std::endl;
			break;
		case 5:
			enabledHacks[AutoCap] = !enabledHacks[AutoCap];
			if (enabledHacks[AutoCap]) {
				std::cout << "Type 0 to not require typing in code, or 1 to require a code: ";
				std::cin >> hacksSettings[NeedCode];
			}
			std::cout << "Auto Cap enabled: " << enabledHacks[AutoCap] << std::endl;
			break;
		case 6:
			enabledHacks[MaxDamage] = !enabledHacks[MaxDamage];
			std::cout << "Enter Damage per round (2000) for instant death: ";
			std::cin >> damageSetting;
			std::cout << "Max Damage enabled: " << enabledHacks[MaxDamage] << std::endl;
			break;
		case 7:
			enabledHacks[MaxROF] = !enabledHacks[MaxROF];
			std::cout << "Enter Rate of fire in rpm: ";
			int rof;
			std::cin >> rof;
			rofSetting = 60 / rof;

			std::cout << "Max Rate Of Fire enabled: " << enabledHacks[MaxROF] << std::endl;
			break;
		case 8:
			enabledHacks[SpeedHack] = !enabledHacks[SpeedHack];
			std::cout << "Speed Hack enabled: " << enabledHacks[SpeedHack] << std::endl;
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

		uintptr_t SetOutlineActive_offset = 0x3C17A0;
		oSetOutlineActive = (tSetOutlineActive)(assemblyAddress + SetOutlineActive_offset);

		uintptr_t FireWeapon_offset = 0x4F38B0;
		oFireWeapon = (tFireWeapon)(assemblyAddress + FireWeapon_offset);

		uintptr_t SetOutlineColor_offset = 0xE71830;
		oSetOutlineColor = (tSetOutlineColor)(assemblyAddress + SetOutlineColor_offset);

		uintptr_t SetCurrentAmmo_offset = 0x2A9FE0;
		oSetCurrentAmmo = (tSetCurrentAmmo)(assemblyAddress + SetCurrentAmmo_offset);

		uintptr_t CodeManagerAwake_offset = 0x3B0470;
		oCodeManagerAwake = (tCodeManagerAwake)(assemblyAddress + CodeManagerAwake_offset);

		uintptr_t CheckNumbers_offset = 0x3B0740;
		oCheckNumbers = (tCheckNumbers)(assemblyAddress + CheckNumbers_offset);

		uintptr_t DoCodeCorrect_offset = 0x3B0AF0;
		oDoCodeCorrect = (tDoCodeCorrect)(assemblyAddress + DoCodeCorrect_offset);

		uintptr_t WarPlayerAwake_offset = 0x3952B0;
		oWarPlayerAwake = (tWarPlayerAwake)(assemblyAddress + WarPlayerAwake_offset);

		uintptr_t SetManualInvincibity_offset = 0x399020;
		oSetManualInvincibility = (tSetManualInvincibility)(assemblyAddress + SetManualInvincibity_offset);

		// Attach Detours
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)oSetOutlineActive, hkSetOutlineActive);
		DetourAttach(&(PVOID&)oFireWeapon, hkFireWeapon);
		DetourAttach(&(PVOID&)oSetOutlineColor, hkSetOutlineColor);
		DetourAttach(&(PVOID&)oCodeManagerAwake, hkCodeManagerAwake);
		DetourAttach(&(PVOID&)oCheckNumbers, hkCheckNumbers);
		DetourAttach(&(PVOID&)oWarPlayerAwake, hkWarPlayerAwake);

		// Speedhack
		Speedhack::Setup();
		Speedhack::SetSpeed(1.0);

		LONG lError = DetourTransactionCommit();
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
		DetourDetach(&(PVOID&)oSetOutlineColor, hkSetOutlineColor);
		DetourDetach(&(PVOID&)oCodeManagerAwake, hkCodeManagerAwake);
		DetourDetach(&(PVOID&)oCheckNumbers, hkCheckNumbers);
		DetourDetach(&(PVOID&)oWarPlayerAwake, hkWarPlayerAwake);

		// Speedhack
		Speedhack::Detach();


		LONG lError = DetourTransactionCommit();
		if (lError != NO_ERROR) {
			MessageBox(HWND_DESKTOP, L"Failed to detour", L"timb3r", MB_OK);
		}
		break;
	}
	}

	return true;
}

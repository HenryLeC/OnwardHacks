// dllmain.cpp : Main Hooking Logic And Input Logic.
#include "pch.h"
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <map>
#include "Detours/detours.h"
#include "Structs.h"
#include "HookHeaders.h"
#include "mem.h"

// Hacks Settings
bool espEnabled = false;
bool multiShotEnabled = false;
int shotsPerBurst = 1;
bool gunHacksEnabled = false;

// Original Params
std::map<uintptr_t, float> defaultDamage = {};
std::map<uintptr_t, float> defaultRof = {};

// SetOutlineActive
void __fastcall hkSetOutlineActive(void* pThis, bool active) {
	//std::cout << pThis << std::endl;
	if (espEnabled) {
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
void __fastcall hkFireWeapon(uintptr_t weapon, void* PlayerSource, void* forward, void* aiSourceId) {
	//std::cout << "Weapon Object: " << (weapon) << std::endl;
	// Infinite Ammo
	bool* infAmmo = (bool*)(weapon + 0xAC);
	// Rof
	float* rof = (float*)(weapon + 0x98);
	// No recoil
	bool* noRcoil = (bool*)(weapon + 0xC3);
	// Damage
	float* damage = (float*)(weapon + 0xF8);

	// Set Gun Params
	if (defaultDamage.find(weapon) == defaultDamage.end()) {
		defaultDamage[weapon] = *damage;
		defaultRof[weapon] = *rof;
	}

	if (gunHacksEnabled) {
		*infAmmo = true;
		*rof = 0.00001;
		*noRcoil = true;
		*damage = 2000;
	}
	else {
		*infAmmo = false;
		*rof = defaultRof[weapon];
		*damage = defaultDamage[weapon];
		*noRcoil = false;
	}
	if (multiShotEnabled) {
		for (int i = shotsPerBurst - 1; i <= 5; i++) {
			oFireWeapon(weapon, PlayerSource, forward, aiSourceId);
		}
	}
	return oFireWeapon(weapon, PlayerSource, forward, aiSourceId);
}

// Code manager Active
void __fastcall hkCodeManagerAwake(uintptr_t pThis) {
	//std::cout << pThis << std::endl;
	return oCodeManagerAwake(pThis);
}

void __fastcall hkCheckNumbers(uintptr_t pThis) {
	return oDoCodeCorrect(pThis);
}

DWORD WINAPI HackThread(HMODULE hModule) {
	//Create Console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONIN$", "r", stdin);

	std::cout << "HackThread is running" << std::endl;
	std::cout << "Ready For Inputs" << std::endl;

	while (true)
	{
		//Keypress listening
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			espEnabled = !espEnabled;
			std::cout << "ESP enabled: " << espEnabled << std::endl;
		}
		else if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			multiShotEnabled = !multiShotEnabled;
			std::cout << "Multi Shot enabled: " << multiShotEnabled << std::endl;
			if (multiShotEnabled) {
				std::cout << "Enter Amount of Bullets Per Burst: ";
				std::cin >> shotsPerBurst;
				std::cout << "Shots Per Burst: " << shotsPerBurst << std::endl;
			}
		}
		else if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			gunHacksEnabled = !gunHacksEnabled;
			std::cout << "Gun Hacks enabled: " << gunHacksEnabled << std::endl;
		}
		else if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
			break;
		}
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

		// Attach Detours
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)oSetOutlineActive, hkSetOutlineActive);
		DetourAttach(&(PVOID&)oFireWeapon, hkFireWeapon);
		DetourAttach(&(PVOID&)oSetOutlineColor, hkSetOutlineColor);
		DetourAttach(&(PVOID&)oCodeManagerAwake, hkCodeManagerAwake);
		DetourAttach(&(PVOID&)oCheckNumbers, hkCheckNumbers);

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

		LONG lError = DetourTransactionCommit();
		if (lError != NO_ERROR) {
			MessageBox(HWND_DESKTOP, L"Failed to detour", L"timb3r", MB_OK);
		}
		break;
	}
	}

	return true;
}

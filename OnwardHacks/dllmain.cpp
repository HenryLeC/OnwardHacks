// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include "Detours/detours.h"
#include "Structs.h"

// Hacks Settings
bool espEnabled = false;
bool multiShotEnabled = false;

// SetOutlineActive
typedef void(__fastcall* tSetOutlineActive)(void*, bool);
tSetOutlineActive oSetOutlineActive;

void __fastcall hkSetOutlineActive(void* pThis, bool active) {
	std::cout << pThis << std::endl;
	if (espEnabled) {
		return oSetOutlineActive(pThis, true);
	}
	else {
		return oSetOutlineActive(pThis, active);
	}
}

// SetOutlineColor
typedef void(__fastcall* tSetOutlineColor)(void*, Color);
tSetOutlineColor oSetOutlineColor;

void __fastcall hkSetOutlineColor(void* pThis, Color color) {
	std::cout << color.r << " " << color.g << " " << color.b << " " << color.a << " " << std::endl;
	oSetOutlineColor(pThis, color);
}

// Fire Weapon
typedef void(__fastcall* tFireWeapon)(void*, void*, void*, void*);
tFireWeapon oFireWeapon;

void __fastcall hkFireWeapon(void* pThis, void* PlayerSource, void* forward, void* aiSourceId) {
	if (multiShotEnabled) {
		for (int i = 0; i <= 5; i++) {
			oFireWeapon(pThis, PlayerSource, forward, aiSourceId);
		}
	}
	return oFireWeapon(pThis, PlayerSource, forward, aiSourceId);
}


DWORD WINAPI HackThread(HMODULE hModule) {
	//Create Console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	//freopen_s(&f, "CONIN$", "r", stdin);

	//std::string espInput = "";
	std::cout << "HackThread is running\n";
	/*std::cout << "Enter True for ESP or False for no ESP" << std::endl;
	std::cin >> espInput;
	std::cout << espInput << std::endl;*/

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
		}
		else if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
			break;
		}
	}

	if (f) {
		fclose(f);
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

		uintptr_t SpectateOutline_SetOutlineActiveRVA = 0x3C17A0;
		oSetOutlineActive = (tSetOutlineActive)(assemblyAddress + SpectateOutline_SetOutlineActiveRVA);

		uintptr_t FireWeapon_offset = 0x4F38B0;
		oFireWeapon = (tFireWeapon)(assemblyAddress + FireWeapon_offset);

		uintptr_t SetOutlineColor_offset = 0xE71830;
		oSetOutlineColor = (tSetOutlineColor)(assemblyAddress + SetOutlineColor_offset);


		// Attach Detours
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)oSetOutlineActive, hkSetOutlineActive);
		DetourAttach(&(PVOID&)oFireWeapon, hkFireWeapon);
		DetourAttach(&(PVOID&)oSetOutlineColor, hkSetOutlineColor);

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

		LONG lError = DetourTransactionCommit();
		if (lError != NO_ERROR) {
			MessageBox(HWND_DESKTOP, L"Failed to detour", L"timb3r", MB_OK);
		}
		break;
	}
	}

	return true;
}

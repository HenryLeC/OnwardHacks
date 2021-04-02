// dllmain.cpp : Main Hooking Logic And Input Logic.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <map>
#include <thread>
#include <fstream>
#include <unordered_map>
#include "Detours/detours.h"
#include "Speedhack.h"
#include "Hooks.h"
#include "../OnwardHacksGUI/gui.h"
#include "../OnwardHacksGUI/HacksSettings.h"


DWORD WINAPI HackThread(HMODULE hModule) {
	//Create Console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONIN$", "r", stdin);

	// Set Registry Key
	std::thread RegPatch(EditRegKeys);
	RegPatch.detach();

	Running = true;

	// Open the Gui Blocking Call
	mainGUI();

	// Disinject Procedure
	Running = false;
	// Kill Reg Patch
	KillRegPatch();
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

		oGetSteamID = (tGetSteamID)(assemblyAddress + GetSteamID_offset);

		oisDeveloper = (tisDeveloper)(assemblyAddress + isDeveloper_offset);

		oGetPenetrationInfo = (tGetPenetrationInfo)(assemblyAddress + GetPenetrationInfo_offset);

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
		DetourAttach(&(PVOID&)oisDeveloper, hkisDeveloper);
		DetourAttach(&(PVOID&)oGetPenetrationInfo, hkGetPenetrationInfo);

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
		DetourDetach(&(PVOID&)oisDeveloper, hkisDeveloper);
		DetourDetach(&(PVOID&)oGetPenetrationInfo, hkGetPenetrationInfo);

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

#pragma once

#include <Windows.h>

namespace Speedhack
{
	extern double speed;
	extern bool attatched;

	DWORD WINAPI _hkGetTickCount();

	DWORD WINAPI _hkGetTickCount64();

	DWORD WINAPI _hkQueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount);


	void Setup();

	void Detach();

	void SetSpeed(float relSpeed);
}
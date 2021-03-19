#include "Speedhack.h"
#include "pch.h"
#include "Detours/detours.h"

namespace Speedhack
{
	double speed = 1.0;
	bool attatched = false;

	typedef DWORD(WINAPI* _tGetTickCount)(void);

	typedef ULONGLONG(WINAPI* _tGetTickCount64)(void);

	typedef BOOL(WINAPI* _tQueryPerformanceCounter)(LARGE_INTEGER*);

	_tGetTickCount _oGetTickCount;
	DWORD _GTC_BaseTime = 0;

	_tGetTickCount64 _oGetTickCount64;
	DWORD _GTC64_BaseTime = 0;

	_tQueryPerformanceCounter _oQueryPerformanceCounter;
	LARGE_INTEGER _QPC_BaseTime = LARGE_INTEGER();

	DWORD WINAPI _hkGetTickCount()
	{
		return _GTC_BaseTime + ((_oGetTickCount() - _GTC_BaseTime) * speed);
	}

	DWORD WINAPI _hkGetTickCount64()
	{
		return _GTC64_BaseTime + ((_oGetTickCount64() - _GTC64_BaseTime) * speed);
	}

	DWORD WINAPI _hkQueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount)
	{
		LARGE_INTEGER x;
		_oQueryPerformanceCounter(&x);
		lpPerformanceCount->QuadPart = _QPC_BaseTime.QuadPart + ((x.QuadPart - _QPC_BaseTime.QuadPart) * speed);
		return TRUE;
	}

	void Setup()
	{
		if (attatched)
			return;

		HMODULE hMod = GetModuleHandle(L"Kernel32.dll");

		if (!hMod)
			return;

		_oGetTickCount = (_tGetTickCount)GetProcAddress(hMod, "GetTickCount");
		_GTC_BaseTime = _oGetTickCount();

		_oGetTickCount64 = (_tGetTickCount64)GetProcAddress(hMod, "GetTickCount64");
		_GTC64_BaseTime = _oGetTickCount64();

		_oQueryPerformanceCounter = (_tQueryPerformanceCounter)GetProcAddress(hMod, "QueryPerformanceCounter");
		_oQueryPerformanceCounter(&_QPC_BaseTime);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)_oGetTickCount, _hkGetTickCount);
		DetourAttach(&(PVOID&)_oGetTickCount64, _hkGetTickCount64);
		DetourAttach(&(PVOID&)_oQueryPerformanceCounter, _hkQueryPerformanceCounter);

		DetourTransactionCommit();

		attatched = true;
	}

	void Detach()
	{
		if (!attatched)
			return;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)_oGetTickCount, _hkGetTickCount);
		DetourDetach(&(PVOID&)_oGetTickCount64, _hkGetTickCount64);
		DetourDetach(&(PVOID&)_oQueryPerformanceCounter, _hkQueryPerformanceCounter);

		DetourTransactionCommit();

		attatched = false;
	}

	void SetSpeed(float relSpeed)
	{
		if (attatched)
		{
			_GTC_BaseTime = _hkGetTickCount();
			_GTC64_BaseTime = _hkGetTickCount64();
			_hkQueryPerformanceCounter(&_QPC_BaseTime);
		}

		speed = relSpeed;
	}
}
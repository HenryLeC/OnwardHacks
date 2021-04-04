#include <Windows.h>
#include "HacksSettings.h"

// Visuals
bool ESP = false;

// Uplink
bool AutoCap = false;
bool AnyCodeCap = false;

// Gun Hacks
bool NoRecoil = false;
bool bMaxDamage = false;
int iMaxDamage = 0;
bool bMaxRPM = false;
int iMaxRPM = 1;
bool FastBurst = false;
int iFastBurst = 0;
bool InfiniteAmmo = false;

// Spoofing
bool SteamIDSpoofer = false;
bool SpoofName = false;
char sSpoofName[128] = "";

// Others
bool InfinitePoints = false;
bool DevMode = false;
bool SpeedHack = false;
float fSpeedHack = 1.0f;
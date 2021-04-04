#include "gui.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <unordered_map>
#include "HacksSettings.h"
#include <string>
#include "ini.h"
#pragma comment(lib, "d3d9.lib")

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

mINI::INIFile file("hacksConfig.ini");
mINI::INIStructure ini;


// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Load Saved Config

bool checkIfExists(std::string category, std::string key) {
    if (ini.has(category)) {
        if (ini[category].has(key)) {
            return true;
        }
    }
    return false;
}

int GetValue(std::string category, std::string key, int def) {
    if (checkIfExists(category, key)) {
        std::string sValue = ini[category][key];
        return stoi(sValue);
    }

    return def;
}

float GetValue(std::string category, std::string key, float def) {
    if (checkIfExists(category, key)) {
        std::string sValue = ini[category][key];
        return stof(sValue);
    }

    return def;
}

char* GetValue(std::string category, std::string key, char* def) {
    if (checkIfExists(category, key)) {
        std::string sValue = ini[category][key];
        char* ret = new char[sValue.length() + 1];
        strcpy_s(ret, sValue.length() + 1, sValue.c_str());
        return ret;
    }

    return def;
}

bool GetValue(std::string category, std::string key, bool def = false) {
    if (checkIfExists(category, key)) {
        std::string sValue = ini[category][key];
        return sValue == "true" ? true : false;
    }

    return def;
}

std::string iniConvert(bool value) {
    return value ? "true" : "false";
}

std::string iniConvert(float value) {
    return std::to_string(value);
}

std::string iniConvert(int value) {
    return std::to_string(value);
}

void loadConfig() {
    file.read(ini);

    // Visuals
    ESP = GetValue("Visuals", "ESP");

    // Uplink
    AutoCap = GetValue("Uplink", "AutoCap");
    AnyCodeCap = GetValue("Uplink", "AnyCode");

    // Gun Hacks
    NoRecoil = GetValue("Gun Hacks", "NoRecoil");
    bMaxDamage = GetValue("Gun Hacks", "bMaxDamage");
    iMaxDamage = GetValue("Gun Hacks", "iMaxDamage", 0);
    bMaxRPM = GetValue("Gun Hacks", "bMaxRPM");
    iMaxRPM = GetValue("Gun Hacks", "iMaxRPM", 0);
    FastBurst = GetValue("Gun Hacks", "FastBurst");
    iFastBurst = GetValue("Gun Hacks", "iFastBurst", 0);
    InfiniteAmmo = GetValue("Gun Hacks", "InfiniteAmmo");

    // Spoofing
    SteamIDSpoofer = GetValue("Spoofing", "SteamID");

    // Others
    InfinitePoints = GetValue("Others", "InfinitePoints");
    DevMode = GetValue("Others", "DevMode");
    SpeedHack = GetValue("Others", "SpeedHack");
    fSpeedHack = GetValue("Others", "fSpeedHack", 1.0f);
}

void saveConfig() {
    file.read(ini);

    // Visuals
    ini["Visuals"]["ESP"] = iniConvert(ESP);

    // Uplink
    ini["Uplink"]["AutoCap"] = iniConvert(AutoCap);
    ini["Uplink"]["AnyCode"] = iniConvert(AnyCodeCap);

    // Gun Hacks
    ini["Gun Hacks"]["NoRecoil"] = iniConvert(NoRecoil);
    ini["Gun Hacks"]["bMaxDamage"] = iniConvert(bMaxDamage);
    ini["Gun Hacks"]["iMaxDamage"] = iniConvert(iMaxDamage);
    ini["Gun Hacks"]["bMaxRPM"] = iniConvert(bMaxRPM);
    ini["Gun Hacks"]["iMaxRPM"] = iniConvert(iMaxRPM);
    ini["Gun Hacks"]["FastBurst"] = iniConvert(FastBurst);
    ini["Gun Hacks"]["iFastBurst"] = iniConvert(iFastBurst);
    ini["Gun Hacks"]["InfiniteAmmo"] = iniConvert(InfiniteAmmo);

    // Spoofing
    ini["Spoofing"]["SteamID"] = iniConvert(SteamIDSpoofer);

    // Others
    ini["Others"]["InfinitePoints"] = iniConvert(InfinitePoints);
    ini["Others"]["DevMode"] = iniConvert(DevMode);
    ini["Others"]["SpeedHack"] = iniConvert(SpeedHack);
    ini["Others"]["fSpeedHack"] = iniConvert(fSpeedHack);

    file.write(ini, true);
}

// Main) code
int mainGUI()
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Onward Hax"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Onward Hax"), WS_OVERLAPPEDWINDOW, 100, 100, 315, 535, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);
    loadConfig();

    // Main loop
    bool done = false;
    while (!done)
    {
        // Handle Messages
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                done = true;
                saveConfig();
            }
                
        }
        if (done)
        {
            break;
        }
        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
        {
            ImGui::ShowDemoWindow();
            //Important
            ImGui::Begin("OnwardHax");
            ImGui::StyleColorsDark;
            ImGui::SetWindowSize(ImVec2(300, 500));
            ImGui::SetWindowPos(ImVec2(0, 0));
            io.ConfigWindowsMoveFromTitleBarOnly = true;
            io.ConfigWindowsResizeFromEdges = false;
 
            //UI
            //Visuals
            ImGui::TextColored(ImVec4(1, 0, 1, 1), "Visuals:");
            ImGui::Indent();
            ImGui::Checkbox("Chams", &ESP);
            ImGui::Unindent();
            ImGui::Text("");

            //Uplink
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Uplink:");
            ImGui::Indent();
            ImGui::Checkbox("Auto-Cap", &AutoCap);
            ImGui::Checkbox("Any Code Cap", &AnyCodeCap);
            ImGui::Unindent();
            ImGui::Text("");

            //Gun Mods
            ImGui::TextColored(ImVec4(0, 1, 1, 1), "Gun Mods:");
            ImGui::Indent();
            ImGui::Checkbox("No Recoil", &NoRecoil);
            ImGui::Checkbox("Set custom damage", &bMaxDamage);
            if (bMaxDamage == true) {
                ImGui::Text("Set custom damage (2000 - instakill)");
                ImGui::SliderInt("###MaxDamage", &iMaxDamage, 1, 2000);}
            ImGui::Checkbox("Gun RPM", &bMaxRPM);
            if (bMaxRPM == true){
                ImGui::Text("Set Gun RPM (2000 - insanly fast)");
                ImGui::SliderInt("###MaxRPM", &iMaxRPM, 1, 4000);}
            ImGui::Checkbox("Fast Burst", &FastBurst);
            if (FastBurst == true){
                ImGui::Text("Set Bursts Per Shot:");
                ImGui::SliderInt("###Fasturst", &iFastBurst, 0, 20);}
            ImGui::Checkbox("Infinite Ammo", &InfiniteAmmo);
            ImGui::Unindent();
            ImGui::Text("");

            //Spoofing
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Spoofing:");
            ImGui::Indent();
            ImGui::Checkbox("SteamID Spoofer enabled", &SteamIDSpoofer);
            ImGui::Checkbox("Spoof Name", &SpoofName);
            if (SpoofName == true) {
            static int item_current_2 = 0;
            ImGui::Combo("lololol", &item_current_2, "red\0blue\0green\0yellow\0purple\0\0");
            ImGui::Text("Enter Spoofed Name: %s", sSpoofName);
            ImGui::InputText("###Spoof", sSpoofName, IM_ARRAYSIZE(sSpoofName));}
            ImGui::Unindent();
            ImGui::Text("");

            //Others
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Others:");
            ImGui::Indent();
            ImGui::Checkbox("Infinite Points", &InfinitePoints);
            ImGui::Checkbox("Unlock All Pens", &DevMode);
            ImGui::Checkbox("Bypass Full Lobbies", &DevMode);
            ImGui::Checkbox("Speed Hack Multiplier", &SpeedHack);
            if (SpeedHack == true) {
                ImGui::SliderFloat("###SpeedHack", &fSpeedHack, 1.0f, 3.0f, "Speed = %.2f");}
            ImGui::Unindent();
            ImGui::Text("");

            if (ImGui::Button("Disinject Hacks")) {
                //Beep(800, 1000);
                done = true;
                saveConfig();
            }
            ImGui::End();
        }

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

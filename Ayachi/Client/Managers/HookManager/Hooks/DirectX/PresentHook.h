#pragma once

#include <Windows.h>
#include <dxgi1_4.h>

#include "../FuncHook.h"

struct ImGuiIO;

class PresentHook : public FuncHook {
private:
    using present_t = HRESULT(__thiscall*)(IDXGISwapChain3*, UINT, UINT);
    static present_t oPresent;
    static WNDPROC originalWndProc;
    static HWND s_window;

    struct GameWindowSearch {
        DWORD pid = 0;
        LONG_PTR bestArea = 0;
        HWND best = nullptr;
    };

    static bool fileExists(const char* path);
    static void loadChineseGlyphs(ImGuiIO& io);
    static bool handleMouseMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK gameWindowEnumProc(HWND hwnd, LPARAM lp);
    static HWND findGameWindow();
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static void buildOverlayUi();
    static void ensureImGuiContext();
    static HRESULT presentCallback(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags);

public:
    static void shutdown();
    PresentHook();
};

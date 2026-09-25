#include "PresentHook.h"

#include <d3d11.h>
#include <d3d12.h>
#include <windowsx.h>
#include <impl/imgui_impl_dx11.h>
#include <impl/imgui_impl_win32.h>

#include "../../../../Client.h"
#include "../../../FontManager/FontManager.h"
#include "../../../InputManager/MouseInput.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../ModuleManager/Modules/Category/Client/ClickGUI.h"
#include "../../../../../Renderer/D3D12Overlay.h"
#include "../../../../../Renderer/ImGuiRender.h"
#include "../../../../../SDK/Runtime/GameData.h"
#include "../../../../../Utils/HookGuard.h"
#include "../../../../../Utils/Logger.h"
#include "../Input/InputPoller.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam);

PresentHook::present_t PresentHook::oPresent = nullptr;
WNDPROC PresentHook::originalWndProc = nullptr;
HWND PresentHook::s_window = nullptr;

bool PresentHook::fileExists(const char* path) {
    const DWORD attrs = GetFileAttributesA(path);
    return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

void PresentHook::loadChineseGlyphs(ImGuiIO& io) {
    ImFont* defaultFont = io.Fonts->AddFontDefault();

    ImFontConfig fontConfig;
    fontConfig.MergeMode = true;
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 1;
    fontConfig.PixelSnapH = true;

    static const ImWchar chineseRanges[] = {
        0x2000, 0x206F,
        0x3000, 0x303F,
        0x4E00, 0x9FFF,
        0
    };

    const char* fontPath = "C:\\Windows\\Fonts\\simhei.ttf";

    if (fileExists(fontPath)) {
        io.Fonts->AddFontFromFileTTF(fontPath, 0.0f, &fontConfig, chineseRanges);
    }

    FontManager::setDefaultFont(defaultFont);
    FontManager::processPendingApply();
}

bool PresentHook::handleMouseMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_POINTERDOWN || msg == WM_POINTERUPDATE || msg == WM_POINTERUP) {
        const UINT pointerId = GET_POINTERID_WPARAM(wParam);
        POINTER_INPUT_TYPE inputType = PT_POINTER;
        if (!GetPointerType(pointerId, &inputType) || inputType != PT_TOUCH)
            return false;
        if (!Mouse::guiOwnsMouse())
            return false;

        if (!IS_POINTER_PRIMARY_WPARAM(wParam))
            return true;

        POINTER_INFO info = {};
        if (!GetPointerInfo(pointerId, &info))
            return true;

        POINT pt = info.ptPixelLocation;
        ScreenToClient(hWnd, &pt);
        const Vec2<float> pos = Mouse::clientToGui(Vec2<float>((float)pt.x, (float)pt.y));

        if (msg == WM_POINTERUPDATE)
            Mouse::pushMove(pos);
        else
            Mouse::pushButton(Mouse::Button::Left, msg == WM_POINTERDOWN, pos);
        return true;
    }

    switch (msg) {
    case WM_MOUSEMOVE:
        if (Mouse::guiOwnsMouse()) {
            Mouse::pushMove(Mouse::clientToGui(
                Vec2<float>((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam))));
        }
        return false;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONUP: {
        if (!Mouse::guiOwnsMouse())
            return false;

        Mouse::Button button = Mouse::Button::Left;
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK || msg == WM_RBUTTONUP)
            button = Mouse::Button::Right;
        else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK || msg == WM_MBUTTONUP)
            button = Mouse::Button::Middle;

        const bool isDown = msg != WM_LBUTTONUP && msg != WM_RBUTTONUP && msg != WM_MBUTTONUP;
        Mouse::pushButton(button, isDown, Mouse::clientToGui(
            Vec2<float>((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam))));
        return true;
    }

    case WM_MOUSEWHEEL: {
        if (!Mouse::guiOwnsMouse())
            return false;
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hWnd, &pt);
        Mouse::pushWheel((float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA,
            Mouse::clientToGui(Vec2<float>((float)pt.x, (float)pt.y)));
        return true;
    }

    default:
        return false;
    }
}

BOOL CALLBACK PresentHook::gameWindowEnumProc(HWND hwnd, LPARAM lp) {
    auto* search = reinterpret_cast<GameWindowSearch*>(lp);
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != search->pid || !IsWindowVisible(hwnd))
        return TRUE;
    if ((GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) != 0 ||
        (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) != 0)
        return TRUE;
    RECT rc = {};
    if (!GetWindowRect(hwnd, &rc))
        return TRUE;
    const LONG_PTR area = static_cast<LONG_PTR>(rc.right - rc.left) * (rc.bottom - rc.top);
    if (area > search->bestArea) {
        search->bestArea = area;
        search->best = hwnd;
    }
    return TRUE;
}

HWND PresentHook::findGameWindow() {
    GameWindowSearch search;
    search.pid = GetCurrentProcessId();
    EnumWindows(gameWindowEnumProc, reinterpret_cast<LPARAM>(&search));
    if (search.best != nullptr)
        return search.best;
    return FindWindowA(nullptr, (LPCSTR)"Minecraft");
}

LRESULT WINAPI PresentHook::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (Addresses::GameCore_handleMouseInput == 0 && Addresses::MinecraftGame_update == 0)
        Mouse::tickCursorArbitration();

    if (Mouse::guiOwnsMouse() && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    if (handleMouseMessage(hWnd, msg, wParam, lParam))
        return 0;
    return CallWindowProc(originalWndProc, hWnd, msg, wParam, lParam);
}

void PresentHook::buildOverlayUi() {
    ImGuiIO& io = ImGui::GetIO();
    RECT clientRect = {};
    if (s_window && GetClientRect(s_window, &clientRect)) {
        const float clientWidth = (float)(clientRect.right - clientRect.left);
        const float clientHeight = (float)(clientRect.bottom - clientRect.top);
        if (clientWidth > 0.f && clientHeight > 0.f) {
            Mouse::setClientToGuiScale(
                Vec2<float>(io.DisplaySize.x / clientWidth, io.DisplaySize.y / clientHeight));
        }
    }

    Mouse::beginFrame();

    InputPoller::poll();

    ImGuiRender::setDrawList(ImGui::GetBackgroundDrawList());

    static ClickGUI* clickGuiMod = ModuleManager::getModule<ClickGUI>();
    if (clickGuiMod) {
        clickGuiMod->render(ImGui::GetBackgroundDrawList());
    }
    ModuleManager::onImGuiRender(ImGui::GetBackgroundDrawList());
    NotificationManager::Render(ImGui::GetBackgroundDrawList());
}

void PresentHook::ensureImGuiContext() {
    static bool done = false;
    if (done)
        return;
    done = true;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    loadChineseGlyphs(io);

    ImGui_ImplWin32_Init(s_window);

    if (s_window && !originalWndProc) {
        SetLastError(0);
        originalWndProc =
            (WNDPROC)SetWindowLongPtr(s_window, GWLP_WNDPROC, (LONG_PTR)WndProc);
        if (originalWndProc == nullptr) {
            LOGE("Failed to install the game window procedure (hwnd=%p, error=%lu); touch input is disabled",
                (void*)s_window, GetLastError());
        }
    }
}

HRESULT PresentHook::presentCallback(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags) {
    if (!Client::isInitialized())
        return oPresent(swapChain, syncInterval, flags);

    HookGuard::Scope guard;
    if (!guard)
        return oPresent(swapChain, syncInterval, flags);

    if (!s_window)
        s_window = findGameWindow();

    RECT rect;
    GetWindowRect(s_window, &rect);


    ID3D12Device* d3d12Device = nullptr;
    ID3D11Device* d3d11Device = nullptr;


    if (SUCCEEDED(swapChain->GetDevice(IID_PPV_ARGS(&d3d12Device)))) {
        ensureImGuiContext();
        FontManager::processPendingApply();

        D3D12Overlay::installCommandQueueHook(d3d12Device);
        D3D12Overlay::render(swapChain, d3d12Device, &buildOverlayUi);

        d3d12Device->Release();
        return oPresent(swapChain, syncInterval, flags);
    }
    else if (SUCCEEDED(swapChain->GetDevice(IID_PPV_ARGS(&d3d11Device)))) {
        static ID3D11DeviceContext* ppContext = nullptr;
        if (!ppContext) {
            d3d11Device->GetImmediateContext(&ppContext);
        }

        ensureImGuiContext();
        FontManager::processPendingApply();

        static bool initContext = false;
        if (!initContext) {
            if (!ImGui_ImplDX11_Init(d3d11Device, ppContext)) {
                LOGE("ImGui D3D11 backend initialization failed");
                d3d11Device->Release();
                return oPresent(swapChain, syncInterval, flags);
            }
            initContext = true;

            DXGI_SWAP_CHAIN_DESC desc{};
            if (SUCCEEDED(swapChain->GetDesc(&desc))) {
                LOGI("Overlay initialized (renderer=D3D11, hwnd=%p, size=%ux%u, format=%d, buffers=%u)",
                    static_cast<void*>(s_window), desc.BufferDesc.Width, desc.BufferDesc.Height,
                    static_cast<int>(desc.BufferDesc.Format), desc.BufferCount);
            }
            else {
                LOGI("Overlay initialized (renderer=D3D11, hwnd=%p)", static_cast<void*>(s_window));
            }
        }


        ID3D11Texture2D* pBackBuffer = nullptr;
        swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

        ID3D11RenderTargetView* mainRenderTargetView = nullptr;
        if (pBackBuffer) {
            d3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGuiRender::beginFrame(d3d11Device, ppContext, pBackBuffer);

        buildOverlayUi();

        ImGui::EndFrame();
        ImGui::Render();


        if (mainRenderTargetView) {
            ppContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            mainRenderTargetView->Release();
        }

        ImGuiRender::endFrame();
        if (pBackBuffer) {
            pBackBuffer->Release();
        }

        d3d11Device->Release();
    }

    return oPresent(swapChain, syncInterval, flags);
}

void PresentHook::shutdown() {
    if (originalWndProc && s_window) {
        SetWindowLongPtr(s_window, GWLP_WNDPROC, (LONG_PTR)originalWndProc);
        originalWndProc = nullptr;
    }
}

PresentHook::PresentHook() {
    OriginFunc = (void*)&oPresent;
    func = (void*)&presentCallback;
}

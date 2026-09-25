#include "HookManager.h"

#include "../../../SDK/Runtime/GameContext.h"
#include "../../../SDK/Runtime/GameData.h"
#include "../../../Utils/Logger.h"

#include <kiero.h>
#include <MinHook.h>

void HookManager::init() {
    if (Addresses::Platform_GameCore_winMain == 0) {
        LOGW("Runtime addresses are not configured; skipping hook initialization");
        return;
    }

    const MH_STATUS initStatus = MH_Initialize();
    if (initStatus != MH_OK && initStatus != MH_ERROR_ALREADY_INITIALIZED) {
        LOGE("MinHook initialization failed: %s", MH_StatusToString(initStatus));
        return;
    }

    registerGameAndInputHooks();
    registerOptionAndWorldHooks();
    registerRenderHooks();
    if (!registerDirectXHooks())
        return;
    registerUIHooks();

    for (int waited = 0; Game::clientInstance == nullptr && waited < 15000; waited += 250) {
        Game::clientInstance = ClientInstance::get();
        if (Game::clientInstance == nullptr)
            Sleep(250);
    }
    if (Game::clientInstance == nullptr) {
        LOGE("ClientInstance was not available after 15 seconds");
        return;
    }

    registerItemRendererHook();
    registerNetworkHooks();

    for (auto* funcHook : hooksCache)
        funcHook->enableHook();

    for (auto* funcHook : hooksCache)
        delete funcHook;
    hooksCache.clear();
}

void HookManager::shutdown() {
    shutdownOverlay();
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);

    kiero::shutdown();

    for (auto* funcHook : hooksCache)
        delete funcHook;
    hooksCache.clear();
}

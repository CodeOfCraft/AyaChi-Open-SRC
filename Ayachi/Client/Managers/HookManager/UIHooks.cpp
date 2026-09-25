#include "HookManager.h"
#include "Hooks/Game/ClientInstanceUpdateHook.h"
#include "Hooks/Render/DrawImageHook.h"
#include "Hooks/Render/DrawNinesliceHook.h"
#include "Hooks/Render/DrawTextHook.h"
#include "Hooks/Render/ScreenViewSetupAndRenderHook.h"
#include "../../../SDK/Runtime/GameData.h"
#include "../../../Utils/Logger.h"

void HookManager::registerUIHooks() {
	if (auto* clientInstanceHook = getHook<ClientInstanceUpdateHook>())
		clientInstanceHook->enableHook();
	if (auto* screenViewHook = getHook<ScreenViewSetupAndRenderHook>();
		screenViewHook != nullptr && screenViewHook->enableHook()) {
		for (int waited = 0;
			ScreenViewSetupAndRenderHook::getUIRenderContextVTable() == nullptr && waited < 5000;
			waited += 50) {
			Sleep(50);
		}

		auto** uiRenderContextVTable = ScreenViewSetupAndRenderHook::getUIRenderContextVTable();
		if (uiRenderContextVTable != nullptr) {
			RequestHook<DrawTextHook>(
				uiRenderContextVTable, GameData::VTableIndex::UIRenderContext::drawText);
			RequestHook<DrawImageHook>(
				uiRenderContextVTable, GameData::VTableIndex::UIRenderContext::drawImage);
			RequestHook<DrawNinesliceHook>(
				uiRenderContextVTable, GameData::VTableIndex::UIRenderContext::drawNineslice);
		}
		else {
			LOGW("MinecraftUIRenderContext was unavailable after 5 seconds; UI draw hooks are disabled");
		}
	}
}

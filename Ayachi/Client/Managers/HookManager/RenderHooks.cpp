#include "HookManager.h"
#include "Hooks/Render/GetFovHook.h"
#include "Hooks/Render/ItemRendererRenderHook.h"
#include "Hooks/Render/LevelRendererRenderLevelHook.h"
#include "Hooks/Render/RenderHitSelectHook.h"
#include "Hooks/Render/RenderItemInHandHook.h"
#include "Hooks/Render/RenderItemInOffhandHook.h"
#include "Hooks/Render/RenderOutlineSelectionHook.h"
#include "Hooks/Render/ScreenViewSetupAndRenderHook.h"
#include "../../../SDK/Runtime/GameContext.h"
#include "../../../SDK/Runtime/GameData.h"
#include "../../../Utils/Logger.h"

void HookManager::registerRenderHooks() {
	RequestHook<GetFovHook>(Addresses::LevelRendererPlayer_getFov);
	RequestHook<LevelRendererRenderLevelHook>(Addresses::LevelRenderer_renderLevel);
	RequestHook<RenderOutlineSelectionHook>(Addresses::LevelRendererPlayer_renderOutlineSelection);
	RequestHook<RenderHitSelectHook>(Addresses::LevelRendererPlayer_renderHitSelect);
	RequestHook<RenderItemInHandHook>(Addresses::ItemInHandRenderer_renderFirstPerson);
	RequestHook<RenderItemInOffhandHook>(Addresses::ItemInHandRenderer_renderOffhandItem);
	RequestHook<ScreenViewSetupAndRenderHook>(Addresses::ScreenView_setupAndRender);
}

void HookManager::registerItemRendererHook() {
	ItemRenderer* itemRenderer = nullptr;
	for (int waited = 0; itemRenderer == nullptr && waited < 15000; waited += 250) {
		itemRenderer = Game::clientInstance->getItemRenderer();
		if (itemRenderer == nullptr)
			Sleep(250);
	}
	if (itemRenderer != nullptr) {
		auto** itemRendererVTable = *reinterpret_cast<uintptr_t***>(itemRenderer);
		RequestHook<ItemRendererRenderHook>(
			itemRendererVTable, GameData::VTableIndex::ItemRenderer::render);
	} else {
		LOGW("ItemRenderer was unavailable after 15 seconds; dropped-item render hook is disabled");
	}
}

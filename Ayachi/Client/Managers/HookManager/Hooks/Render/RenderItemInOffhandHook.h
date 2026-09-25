#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Render/BaseActorRenderContext.h"
#include "../../../../../SDK/Render/ItemInHandRenderer.h"
#include "../../../../../SDK/World/Actor/Player/Player.h"
#include "../../../../../Utils/HookGuard.h"

class RenderItemInOffhandHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		ItemInHandRenderer*, BaseActorRenderContext&, Player&, ItemContextFlags);
	static inline func_t oFunc = nullptr;

	static void renderOffhandItemCallback(
		ItemInHandRenderer* renderer,
		BaseActorRenderContext& renderContext,
		Player& player,
		ItemContextFlags itemFlags) {
		HookGuard::Scope guard;
		if (!guard) {
			oFunc(renderer, renderContext, player, itemFlags);
			return;
		}

		bool cancel = false;
		ModuleManager::onRenderItemInOffhand(
			renderer, renderContext, player, itemFlags, cancel);
		if (!cancel)
			oFunc(renderer, renderContext, player, itemFlags);
	}

public:
	RenderItemInOffhandHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&renderOffhandItemCallback);
	}
};

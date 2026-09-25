#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Render/BaseActorRenderContext.h"
#include "../../../../../SDK/Render/ItemInHandRenderer.h"
#include "../../../../../SDK/Render/Matrix.h"
#include "../../../../../Utils/HookGuard.h"

class RenderItemInHandHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		ItemInHandRenderer*, BaseActorRenderContext&, const Matrix&, ItemContextFlags);
	static inline func_t oFunc = nullptr;

	static void renderFirstPersonCallback(
		ItemInHandRenderer* renderer,
		BaseActorRenderContext& renderContext,
		const Matrix& previousProjection,
		ItemContextFlags itemFlags) {
		HookGuard::Scope guard;
		if (!guard) {
			oFunc(renderer, renderContext, previousProjection, itemFlags);
			return;
		}

		bool cancel = false;
		ModuleManager::onRenderItemInHand(
			renderer, renderContext, previousProjection, itemFlags, cancel);
		if (!cancel)
			oFunc(renderer, renderContext, previousProjection, itemFlags);
	}

public:
	RenderItemInHandHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&renderFirstPersonCallback);
	}
};

#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Render/BaseActorRenderContext.h"
#include "../../../../../SDK/Render/LevelRendererPlayer.h"
#include "../../../../../SDK/World/Level/BlockSource.h"
#include "../../../../../Utils/HookGuard.h"

class RenderHitSelectHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		const LevelRendererPlayer*, BaseActorRenderContext&, BlockSource&, const BlockPos&, bool);
	static inline func_t oFunc = nullptr;

	static void renderHitSelectCallback(
		const LevelRendererPlayer* levelRenderer,
		BaseActorRenderContext& renderContext,
		BlockSource& blockSource,
		const BlockPos& position,
		bool fancyGraphics) {
		HookGuard::Scope guard;
		if (!guard) {
			oFunc(levelRenderer, renderContext, blockSource, position, fancyGraphics);
			return;
		}

		bool cancel = false;
		ModuleManager::onRenderHitSelect(position, cancel);
		if (!cancel)
			oFunc(levelRenderer, renderContext, blockSource, position, fancyGraphics);
	}

public:
	RenderHitSelectHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&renderHitSelectCallback);
	}
};

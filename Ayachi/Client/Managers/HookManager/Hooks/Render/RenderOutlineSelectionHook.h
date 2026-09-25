#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Render/BaseActorRenderContext.h"
#include "../../../../../SDK/Render/LevelRendererPlayer.h"
#include "../../../../../SDK/World/Level/Block/Block.h"
#include "../../../../../SDK/World/Level/BlockSource.h"
#include "../../../../../Utils/HookGuard.h"

class RenderOutlineSelectionHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		const LevelRendererPlayer*, BaseActorRenderContext&, const Block&, BlockSource&, const BlockPos&);
	static inline func_t oFunc = nullptr;

	static void renderOutlineSelectionCallback(
		const LevelRendererPlayer* levelRenderer,
		BaseActorRenderContext& renderContext,
		const Block& block,
		BlockSource& blockSource,
		const BlockPos& position) {
		HookGuard::Scope guard;
		if (!guard) {
			oFunc(levelRenderer, renderContext, block, blockSource, position);
			return;
		}

		bool cancel = false;
		ModuleManager::onRenderOutlineSelection(block, blockSource, position, cancel);
		if (!cancel)
			oFunc(levelRenderer, renderContext, block, blockSource, position);
	}

public:
	RenderOutlineSelectionHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&renderOutlineSelectionCallback);
	}
};

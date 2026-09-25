#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Render/LevelRenderer.h"
#include "../../../../../SDK/Render/ScreenContext.h"
#include "../../../../../Utils/HookGuard.h"

class LevelRendererRenderLevelHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(LevelRenderer*, ScreenContext*, void*);
	static inline func_t oFunc = nullptr;

	static void renderLevelCallback(
		LevelRenderer* levelRenderer, ScreenContext* screenContext, void* frameData) {
		HookGuard::Scope guard;
		oFunc(levelRenderer, screenContext, frameData);
		if (guard)
			ModuleManager::onLevelRender(levelRenderer, screenContext);
	}

public:
	LevelRendererRenderLevelHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&renderLevelCallback);
	}
};

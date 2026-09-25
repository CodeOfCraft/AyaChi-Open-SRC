#pragma once

#include <atomic>

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../Renderer/MCRender.h"
#include "../../../../../Utils/HookGuard.h"

class ScreenViewSetupAndRenderHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(ScreenView*, MinecraftUIRenderContext*);
	static inline func_t oFunc;
	static inline std::atomic<uintptr_t**> uiRenderContextVTable{ nullptr };

	static void setupAndRenderCallback(ScreenView* view, MinecraftUIRenderContext* context) {
		HookGuard::Scope guard;
		if (context != nullptr) {
			auto** vtable = *reinterpret_cast<uintptr_t***>(context);
			uintptr_t** expected = nullptr;
			uiRenderContextVTable.compare_exchange_strong(expected, vtable);
		}
		oFunc(view, context);

		if (guard && MCRender::beginFrame(view, context)) {
			ModuleManager::onMCRender(context);
			MCRender::endFrame();
		}
	}

public:
	[[nodiscard]] static uintptr_t** getUIRenderContextVTable() {
		return uiRenderContextVTable.load(std::memory_order_acquire);
	}

	ScreenViewSetupAndRenderHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&setupAndRenderCallback);
	}
};

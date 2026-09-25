#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Render/MinecraftUIRenderContext.h"
#include "../../../../../Utils/HookGuard.h"

struct NinesliceInfo;
namespace mce {
	struct ClientTexture;
}

class DrawNinesliceHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		MinecraftUIRenderContext*, const mce::ClientTexture&, const NinesliceInfo&);
	static inline func_t oFunc = nullptr;

	static void drawNinesliceCallback(
		MinecraftUIRenderContext* context,
		const mce::ClientTexture& texture,
		const NinesliceInfo& info) {
		HookGuard::Scope guard;
		if (!guard) {
			oFunc(context, texture, info);
			return;
		}

		bool cancel = false;
		ModuleManager::onDrawNineslice(context, texture, info, cancel);
		if (!cancel)
			oFunc(context, texture, info);
	}

public:
	DrawNinesliceHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&drawNinesliceCallback);
	}
};

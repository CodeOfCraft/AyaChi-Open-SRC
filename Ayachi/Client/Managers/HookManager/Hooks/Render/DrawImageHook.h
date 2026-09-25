#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Render/MinecraftUIRenderContext.h"
#include "../../../../../Utils/HookGuard.h"

namespace mce {
	struct ClientTexture;
}

class DrawImageHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		MinecraftUIRenderContext*,
		const mce::ClientTexture&,
		const glm::vec2&,
		const glm::vec2&,
		const glm::vec2&,
		const glm::vec2&,
		bool);
	static inline func_t oFunc = nullptr;

	static void drawImageCallback(
		MinecraftUIRenderContext* context,
		const mce::ClientTexture& texture,
		const glm::vec2& position,
		const glm::vec2& size,
		const glm::vec2& uv,
		const glm::vec2& uvSize,
		bool colorCorrected) {
		HookGuard::Scope guard;
		if (!guard) {
			oFunc(context, texture, position, size, uv, uvSize, colorCorrected);
			return;
		}

		bool cancel = false;
		ModuleManager::onDrawImage(
			context, texture, position, size, uv, uvSize, colorCorrected, cancel);
		if (!cancel)
			oFunc(context, texture, position, size, uv, uvSize, colorCorrected);
	}

public:
	DrawImageHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&drawImageCallback);
	}
};

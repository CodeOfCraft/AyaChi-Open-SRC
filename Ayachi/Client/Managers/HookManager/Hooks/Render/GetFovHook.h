#pragma once

#include "../FuncHook.h"
#include "../../../../../SDK/Runtime/Addresses.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Render/LevelRendererPlayer.h"
#include "../../../../../Utils/HookGuard.h"

class GetFovHook : public FuncHook {
private:
	using func_t = float(__fastcall*)(LevelRendererPlayer*, float, bool);
	static inline func_t oFunc = nullptr;

	static float getFovCallback(
		LevelRendererPlayer* renderer, float frameAlpha, bool enableVariableFov) {
		HookGuard::Scope guard;
		float fov = oFunc(renderer, frameAlpha, enableVariableFov);
		const uintptr_t cameraReturn = Addresses::LevelRendererPlayer_getFovForCameraReturn;
		if (guard && (cameraReturn == 0 ||
			reinterpret_cast<uintptr_t>(_ReturnAddress()) == cameraReturn)) {
			ModuleManager::onGetFov(fov);
		}
		return fov;
	}

public:
	GetFovHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&getFovCallback);
	}
};

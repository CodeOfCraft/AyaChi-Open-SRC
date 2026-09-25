#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../Utils/HookGuard.h"

class GetGammaHook : public FuncHook {
private:
	using func_t = float(__fastcall*)(void*, void*);
	static inline func_t oFunc = nullptr;

	static float getGammaCallback(void* options, void* optionInfo) {
		HookGuard::Scope guard;
		float gamma = oFunc(options, optionInfo);
		if (guard)
			ModuleManager::onGetGamma(gamma);
		return gamma;
	}

public:
	GetGammaHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&getGammaCallback);
	}
};

#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../Utils/HookGuard.h"

class GetPerspectiveHook : public FuncHook {
private:
	using func_t = int(__fastcall*)(void*);
	static inline func_t oFunc = nullptr;

	static int getPerspectiveCallback(void* options) {
		HookGuard::Scope guard;
		int perspective = oFunc(options);
		if (guard)
			ModuleManager::onGetPerspective(perspective);
		return perspective;
	}

public:
	GetPerspectiveHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&getPerspectiveCallback);
	}
};

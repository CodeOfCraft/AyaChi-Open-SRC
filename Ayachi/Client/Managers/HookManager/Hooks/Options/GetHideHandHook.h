#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../Utils/HookGuard.h"

class GetHideHandHook : public FuncHook {
private:
	using func_t = bool(__fastcall*)(void*);
	static inline func_t oFunc = nullptr;

	static bool getHideHandCallback(void* options) {
		HookGuard::Scope guard;
		bool hideHand = oFunc(options);
		if (guard)
			ModuleManager::onGetHideHand(hideHand);
		return hideHand;
	}

public:
	GetHideHandHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&getHideHandCallback);
	}
};

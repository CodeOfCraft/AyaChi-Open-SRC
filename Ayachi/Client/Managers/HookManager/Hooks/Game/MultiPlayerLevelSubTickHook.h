#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/World/Level/Level.h"
#include "../../../../../Utils/HookGuard.h"

class MultiPlayerLevelSubTickHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(Level*);
	static inline func_t oFunc;

	static void subTickCallback(Level* level) {
		HookGuard::Scope guard;
		if (guard)
			ModuleManager::onTick(level);

		oFunc(level);
	}

public:
	MultiPlayerLevelSubTickHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&subTickCallback;
	}
};

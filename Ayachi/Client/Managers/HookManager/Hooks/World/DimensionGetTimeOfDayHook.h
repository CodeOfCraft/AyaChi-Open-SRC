#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../Utils/HookGuard.h"

class Dimension;

class DimensionGetTimeOfDayHook : public FuncHook {
private:
	using func_t = float(__fastcall*)(Dimension*, int, float);
	static inline func_t oFunc = nullptr;

	static float getTimeOfDayCallback(Dimension* dimension, int time, float partialTick) {
		HookGuard::Scope guard;
		float result = oFunc(dimension, time, partialTick);
		if (guard)
			ModuleManager::onGetTimeOfDay(result);
		return result;
	}

public:
	DimensionGetTimeOfDayHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&getTimeOfDayCallback);
	}
};

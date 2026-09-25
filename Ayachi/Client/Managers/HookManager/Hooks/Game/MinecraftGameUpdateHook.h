#pragma once
#include "../FuncHook.h"
#include "../../../InputManager/MouseInput.h"
#include "../../../../Client.h"

class MinecraftGameUpdateHook : public FuncHook {
private:
	using func_t = __int64(__fastcall*)(void*);
	static inline func_t oFunc;

	static __int64 updateCallback(void* minecraftGame) {
		const __int64 result = oFunc(minecraftGame);

		if (Client::isInitialized())
			Mouse::tickCursorArbitration();

		return result;
	}

public:
	MinecraftGameUpdateHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&updateCallback;
	}
};

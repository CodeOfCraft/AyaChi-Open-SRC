#pragma once
#include "../FuncHook.h"
#include "../../../InputManager/MouseInput.h"
#include "../../../../Client.h"

class ApplyTurnDeltaHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(void*, const Vec2<float>&);
	static inline func_t oFunc;

	static void applyTurnDeltaCallback(void* player, const Vec2<float>& delta) {
		if (Client::isInitialized() && Mouse::guiOwnsMouse())
			return;

		oFunc(player, delta);
	}

public:
	ApplyTurnDeltaHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&applyTurnDeltaCallback;
	}
};

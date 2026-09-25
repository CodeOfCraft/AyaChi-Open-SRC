#pragma once
#include "../FuncHook.h"
#include "../../../InputManager/MouseInput.h"
#include "../../../../Client.h"
#include "../../../../../SDK/Runtime/GameContext.h"
#include "../../../../../SDK/Input/MouseDevice.h"

class MouseInputHook : public FuncHook {
private:
	using func_t = bool(__fastcall*)(void*, void*, void*);
	static inline func_t oFunc;

	static bool mouseInputCallback(void* a1, void* a2, void* a3) {
		if (!Client::isInitialized())
			return oFunc(a1, a2, a3);

		Mouse::tickCursorArbitration();

		const bool divert = Mouse::guiOwnsMouse();
		if (divert)
			Mouse::divertGameMouseInput();

		const bool result = oFunc(a1, a2, a3);

		if (divert)
			Mouse::divertGameMouseInput();

		return result;
	}

public:
	MouseInputHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&mouseInputCallback;
	}
};

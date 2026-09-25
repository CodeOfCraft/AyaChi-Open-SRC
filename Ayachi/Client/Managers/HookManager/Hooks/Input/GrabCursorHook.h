#pragma once
#include "../FuncHook.h"
#include "../../../InputManager/MouseInput.h"
#include "../../../../Client.h"

class GrabCursorHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(void*);
	static inline func_t oFunc;

	static void grabCursorCallback(void* clientInstance) {
		if (Client::isInitialized() && Mouse::guiOwnsMouse())
			return;

		oFunc(clientInstance);
	}

public:
	GrabCursorHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&grabCursorCallback;
	}
};

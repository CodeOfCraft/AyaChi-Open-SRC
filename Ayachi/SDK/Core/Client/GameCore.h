#pragma once
#include <string>
#include "Platform_GameCore.h"

class GameCore {
public:
	CLASS_MEMBER(std::string, dataPath, 0x470);
	CLASS_MEMBER(HWND, hwnd, 0x7E0);

	static GameCore* get() {
		Platform_GameCore* platform = Platform_GameCore::get();
		return platform ? platform->gameCore : nullptr;
	}
};

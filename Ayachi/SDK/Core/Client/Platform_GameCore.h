#pragma once
#include "../../../Utils/MemoryUtil.h"
#include "MinecraftGame.h"

class GameCore;

class Platform_GameCore {
public:
	CLASS_MEMBER(MinecraftGame*, minecraftGame, 0x18);
	CLASS_MEMBER(GameCore*, gameCore, 0xB0);

	static Platform_GameCore* get() {
		const uintptr_t winMainSlot = Addresses::Platform_GameCore_winMain;
		if (winMainSlot == 0)
			return nullptr;

		void* winMain = *reinterpret_cast<void**>(winMainSlot);
		if (winMain == nullptr)
			return nullptr;

		return *reinterpret_cast<Platform_GameCore**>(reinterpret_cast<uintptr_t>(winMain) + 0x8);
	}
};

#pragma once
#include <map>
#include <memory>
#include <string>
#include "../../../Utils/MemoryUtil.h"

class ClientInstance;
class GameRenderer;

class MinecraftGame {
public:
	bool isCursorGrabbed() {
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x1D8);
	}

	CLASS_MEMBER(std::string, xuid, 0x250);
	CLASS_MEMBER(GameRenderer*, gameRenderer, 0x1318);

	ClientInstance* getPrimaryClientInstance() {
		auto& map = *reinterpret_cast<std::map<uint8_t, std::shared_ptr<ClientInstance>>*>(
			reinterpret_cast<uintptr_t>(this) + 0x938);
		auto it = map.find(0);
		return it != map.end() ? it->second.get() : nullptr;
	}
};

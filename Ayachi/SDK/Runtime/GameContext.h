#pragma once

#include <cstdarg>
#include <cstdio>
#include <string>

#include "../Core/Client/ClientInstance.h"

namespace Game {
	inline ClientInstance* clientInstance = nullptr;
	inline void* keyMapPtr = nullptr;

	[[nodiscard]] inline ClientInstance* getClientInstance() {
		if (clientInstance == nullptr)
			clientInstance = ClientInstance::get();
		return clientInstance;
	}

	[[nodiscard]] inline GuiData* getGuiData() {
		ClientInstance* instance = getClientInstance();
		return instance != nullptr ? instance->getGuiData() : nullptr;
	}

	inline void DisplayClientMessage(const std::string& message) {
		GuiData* guiData = getGuiData();
		if (guiData != nullptr)
			guiData->displayClientMessage(message);
	}

	inline void DisplayClientMessage(const char* fmt, ...) {
		if (fmt == nullptr)
			return;

		char message[300]{};
		va_list args;
		va_start(args, fmt);
		_vsnprintf_s(message, sizeof(message), _TRUNCATE, fmt, args);
		va_end(args);

		DisplayClientMessage(std::string(message));
	}

	inline bool canUseMoveKeys() {
		ClientInstance* instance = getClientInstance();
		return instance != nullptr && instance->getMouseGrabbed();
	}

	[[nodiscard]] inline LocalPlayer* getLocalPlayer() {
		ClientInstance* instance = getClientInstance();
		return instance != nullptr ? instance->getLocalPlayer() : nullptr;
	}

	[[nodiscard]] inline Level* getLevel() {
		ClientInstance* instance = getClientInstance();
		return instance != nullptr ? instance->getLevel() : nullptr;
	}
}

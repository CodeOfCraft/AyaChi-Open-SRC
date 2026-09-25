#pragma once

#include <optional>

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Core/GUI/GuiData.h"
#include "../../../../../Utils/HookGuard.h"

class DisplayClientMessageHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		GuiData*, const std::string&, std::optional<std::string>, bool);
	static inline func_t oFunc = nullptr;

	static void displayClientMessageCallback(
		GuiData* guiData,
		const std::string& message,
		std::optional<std::string> secondary,
		bool forceVisible) {
		HookGuard::Scope guard;
		if (!guard) {
			oFunc(guiData, message, std::move(secondary), forceVisible);
			return;
		}

		std::string mutableMessage = message;
		bool cancel = false;
		ModuleManager::onDisplayClientMessage(mutableMessage, cancel);
		if (!cancel)
			oFunc(guiData, mutableMessage, std::move(secondary), forceVisible);
	}

public:
	DisplayClientMessageHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&displayClientMessageCallback);
	}
};

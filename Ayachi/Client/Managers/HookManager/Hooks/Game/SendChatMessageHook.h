#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../CommandManager/CommandManager.h"
#include "../../../../../Utils/HookGuard.h"

class SendChatMessageHook : public FuncHook {
private:
	using func_t = uint8_t(__fastcall*)(void*, const std::string&);
	static inline func_t oFunc = nullptr;

	static uint8_t sendChatMessageCallback(void* screenModel, const std::string& message) {
		HookGuard::Scope guard;
		if (!guard)
			return oFunc(screenModel, message);
		if (!message.empty() && message.front() == CommandManager::prefix) {
			CommandManager::execute(message);
			return 0;
		}

		std::string mutableMessage = message;
		bool cancel = false;
		ModuleManager::onSendChatMessage(mutableMessage, cancel);
		return cancel ? 0 : oFunc(screenModel, mutableMessage);
	}

public:
	SendChatMessageHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&sendChatMessageCallback);
	}
};

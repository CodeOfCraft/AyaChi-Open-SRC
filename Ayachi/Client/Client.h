#pragma once
#include <atomic>
#include "Managers/ConfigManager/ConfigManager.h"
#include "Managers/ModuleManager/ModuleManager.h"
#include "Managers/CommandManager/CommandManager.h"
#include "Managers/HookManager/HookManager.h"
#include "Managers/NotificationManager/NotificationManager.h"

class Client {
private:
	static std::atomic<bool> clientInitialized;
	static std::string clientName;
	static std::string version;
public:
	static std::string getVersion() { return version; };
	static inline bool isInitialized() { return clientInitialized.load(std::memory_order_acquire); }
	static std::string getClientName() { return clientName; }
	static void setClientName(std::string newName) { clientName = newName;  }
	static void DisplayClientMessage(const char* fmt, ...);
	static void init();
	static void shutdown();
};

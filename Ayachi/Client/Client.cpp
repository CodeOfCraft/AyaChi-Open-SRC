#include "Client.h"

#include <atomic>
#include "../SDK/Runtime/GameData.h"
#include "../Utils/HookGuard.h"
#include "Managers/FontManager/FontManager.h"
#include "../Utils/FileUtil.h"
#include "../Utils/Logger.h"

std::atomic<bool> Client::clientInitialized{ false };
std::string Client::clientName = "Ayachi";
std::string Client::version = "1.1";

void Client::DisplayClientMessage(const char* fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	char message[300];
	_vsnprintf_s(message, sizeof(message), _TRUNCATE, fmt, arg);
	va_end(arg);

	char headerCStr[50];
	_snprintf_s(headerCStr, sizeof(headerCStr), _TRUNCATE, "%s[%s%s%s] %s", MCTF::GRAY, MCTF::BLUE, getClientName().c_str(), MCTF::GRAY, MCTF::WHITE);

	std::string messageStr = std::string(headerCStr) + message;
	Game::DisplayClientMessage(messageStr);
}

void Client::init() {
	const auto initStarted = std::chrono::steady_clock::now();
	std::string ClientPath = FileUtil::getClientPath();
	if (!FileUtil::doesFilePathExist(ClientPath)) {
		FileUtil::createPath(ClientPath);
	}
	if (!FileUtil::doesFilePathExist(ClientPath + "Maparts\\")) {
		FileUtil::createPath(ClientPath + "Maparts\\");
	}
	if (!FileUtil::doesFilePathExist(ClientPath + "Assets\\")) {
		FileUtil::createPath(ClientPath + "Assets\\");
	}
	Logger::init();
	FontManager::init();
	LOGI("Initialization started (version=%s, pid=%lu, target=26.40)",
		getVersion().c_str(), GetCurrentProcessId());
	ConfigManager::init();

	GameData::init();
	MCTF::initMCTF();

	ModuleManager::init();
	CommandManager::init();
	HookManager::init();

	if (Game::clientInstance == nullptr) {
		HookGuard::beginShutdownAndDrain(2000);
		HookManager::shutdown();
		ModuleManager::shutdown();
		CommandManager::shutdown();
		FontManager::shutdown();
		Logger::shutdown();
		return;
	}

	clientInitialized.store(true, std::memory_order_release);
	NotificationManager::addNotifBox("Ayachi initialized", 5.f);

	if (!ConfigManager::doesConfigExist("default"))
		ConfigManager::createNewConfig("default");

	ConfigManager::loadConfig("default");
	const auto initDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now() - initStarted).count();
	LOGI("Client initialized (duration=%lld ms, instance=%p, modules=%zu, config=default)",
		initDuration, static_cast<void*>(Game::clientInstance), ModuleManager::moduleList.size());
}

void Client::shutdown() {
	static std::atomic_bool shutdownStarted{ false };
	if (shutdownStarted.exchange(true))
		return;

	ConfigManager::saveConfig();

	clientInitialized.store(false, std::memory_order_release);

	if (!HookGuard::beginShutdownAndDrain(2000))
		LOGE("Timed out draining in-flight hook callbacks; continuing teardown anyway.");

	HookManager::shutdown();
	ModuleManager::shutdown();
	CommandManager::shutdown();
	FontManager::shutdown();
	Logger::shutdown();
}

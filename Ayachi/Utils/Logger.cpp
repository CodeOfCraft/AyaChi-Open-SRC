#include "Logger.h"
#include "FileUtil.h"
#include <Windows.h>
#include <cstdarg>

void Logger::init() {
	std::lock_guard<std::mutex> lock(logMutex);

	logPath = FileUtil::getClientPath() + "Ayachi.log";

	logStream.open(logPath, std::ios::out | std::ios::trunc);
	if (logStream.is_open()) {
		initialized = true;
	}
	else {
		CloseHandle(CreateFileA(logPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
		logStream.open(logPath, std::ios::out | std::ios::app);
		if (logStream.is_open()) {
			initialized = true;
		}
	}

}

void Logger::shutdown() {
	std::lock_guard<std::mutex> lock(logMutex);

	if (initialized && logStream.is_open()) {
		logStream.close();
	}
	initialized = false;
}

void Logger::log(LogLevel level, const char* fmt, ...) {
	std::lock_guard<std::mutex> lock(logMutex);

	if (!initialized || !logStream.is_open())
		return;

	va_list args;
	va_start(args, fmt);
	char message[1024];
	vsnprintf(message, sizeof(message), fmt, args);
	va_end(args);

	logStream << "[" << getTimestamp() << "] "
		<< "[" << levelToString(level) << "] "
		<< message << std::endl;
	logStream.flush();
}

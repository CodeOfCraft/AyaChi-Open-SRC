#pragma once
#include <string>
#include <chrono>
#include <fstream>
#include <mutex>

enum class LogLevel {
	INFO = 0,
	WARN = 1,
	ERR = 2,
	DEBUG = 3
};

class Logger {
private:
	static inline std::string logPath;
	static inline std::ofstream logStream;
	static inline std::mutex logMutex;
	static inline bool initialized = false;

	static const char* levelToString(LogLevel level) {
		switch (level) {
		case LogLevel::INFO:  return "INFO";
		case LogLevel::WARN:  return "WARN";
		case LogLevel::ERR: return "ERROR";
		case LogLevel::DEBUG: return "DEBUG";
		default:              return "NONE";
		}
	}

	static std::string getTimestamp() {
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			now.time_since_epoch()) % 1000;

		std::tm tm;
		localtime_s(&tm, &time);

		char buf[32];
		snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03lld",
			tm.tm_hour, tm.tm_min, tm.tm_sec, ms.count());
		return buf;
	}

public:
	static void init();
	static void shutdown();
	static void log(LogLevel level, const char* fmt, ...);
};

#define logF(...) Logger::log(LogLevel::DEBUG, __VA_ARGS__)
#define LOGI(...) Logger::log(LogLevel::INFO, __VA_ARGS__)
#define LOGW(...) Logger::log(LogLevel::WARN, __VA_ARGS__)
#define LOGE(...) Logger::log(LogLevel::ERR, __VA_ARGS__)
#define LOGD(...) Logger::log(LogLevel::DEBUG, __VA_ARGS__)

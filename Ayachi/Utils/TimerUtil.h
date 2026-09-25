#pragma once
#include <chrono>

namespace TimerUtil {
	inline unsigned __int64 getCurrentMs() {
		return static_cast<unsigned __int64>(std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count());
	}
	template<typename T>
	inline float getTime() {
		static const auto startTime = std::chrono::steady_clock::now();
		const auto currentTime = std::chrono::steady_clock::now();
		return static_cast<float>(std::chrono::duration_cast<T>(currentTime - startTime).count());
	}
	template<typename T>
	inline bool hasReached(float* lastScan, float timeToReach) {
		const float currentTime = getTime<T>();
		if (currentTime - *lastScan < timeToReach) return false;
		*lastScan = currentTime;
		return true;
	}
}

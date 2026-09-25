#pragma once
#include <atomic>
#include <chrono>
#include <thread>

namespace HookGuard {
	inline std::atomic<int> activeCalls{ 0 };
	inline std::atomic<bool> shuttingDown{ false };

	class Scope {
	public:
		Scope() {
			if (shuttingDown.load(std::memory_order_acquire))
				return;

			activeCalls.fetch_add(1, std::memory_order_acq_rel);

			if (shuttingDown.load(std::memory_order_seq_cst)) {
				activeCalls.fetch_sub(1, std::memory_order_acq_rel);
				return;
			}

			entered = true;
		}

		~Scope() {
			if (entered)
				activeCalls.fetch_sub(1, std::memory_order_acq_rel);
		}

		Scope(const Scope&) = delete;
		Scope& operator=(const Scope&) = delete;

		explicit operator bool() const { return entered; }

	private:
		bool entered = false;
	};

	inline bool beginShutdownAndDrain(int timeoutMs) {
		shuttingDown.store(true, std::memory_order_seq_cst);

		const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
		while (activeCalls.load(std::memory_order_acquire) > 0) {
			if (std::chrono::steady_clock::now() >= deadline)
				return false;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return true;
	}
}

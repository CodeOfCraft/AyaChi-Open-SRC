#include <Windows.h>
#include <atomic>
#include <chrono>
#include <cstdio>
#include "Client/Client.h"

static std::atomic_bool g_ejectRequested{ false };
static HANDLE g_ejectEvent = nullptr;

static void tryOpenEjectEvent() {
	if (g_ejectEvent)
		return;

	wchar_t name[128];
	swprintf_s(name, L"Global\\Ayachi.Eject.%lu", GetCurrentProcessId());
	g_ejectEvent = OpenEventW(SYNCHRONIZE, FALSE, name);
}

static void cleanupEjectEvent() {
	if (g_ejectEvent) {
		CloseHandle(g_ejectEvent);
		g_ejectEvent = nullptr;
	}
}

static bool isEjectRequested() {
	if (g_ejectRequested.load(std::memory_order_acquire))
		return true;

	tryOpenEjectEvent();
	if (g_ejectEvent && WaitForSingleObject(g_ejectEvent, 0) == WAIT_OBJECT_0) {
		g_ejectRequested.store(true, std::memory_order_release);
		return true;
	}
	return false;
}

extern "C" __declspec(dllexport) DWORD WINAPI Ayachi_RequestEject(LPVOID) {
	g_ejectRequested.store(true, std::memory_order_release);
	return 1;
}

DWORD WINAPI initClient(LPVOID lpParameter) {
	Client::init();

	const float targetTickRate = 50.0f;
	auto lastTick = std::chrono::high_resolution_clock::now();

	while (Client::isInitialized()) {
		if (isEjectRequested()) {
			Client::shutdown();
			break;
		}

		auto now = std::chrono::high_resolution_clock::now();
		float deltaMs = std::chrono::duration<float, std::milli>(now - lastTick).count();

		if (deltaMs >= targetTickRate) {
			ModuleManager::onClientTick();
			lastTick = now;
		}

		Sleep(1);
	}

	cleanupEjectEvent();
	Sleep(50);

	FreeLibraryAndExitThread((HMODULE)lpParameter, 1);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, initClient, hModule, 0, nullptr);
	}
	return TRUE;
}

#pragma once
#include <vector>
#include "Hooks/FuncHook.h"
#include "../../../Utils/MemoryUtil.h"

class HookManager {
public:
	static void init();
	static void shutdown();
private:
	static inline std::vector<FuncHook*> hooksCache;
	static void registerGameAndInputHooks();
	static void registerOptionAndWorldHooks();
	static void registerRenderHooks();
	static bool registerDirectXHooks();
	static void registerUIHooks();
	static void registerItemRendererHook();
	static void registerNetworkHooks();
	static void shutdownOverlay();

	template<typename Hook>
	static void RequestHook(uintptr_t address) {
		if (!address)
			return;

		Hook* funcHook = new Hook();
		funcHook->name = typeid(Hook).name();
		funcHook->address = address;
		funcHook->onHookRequest();

		hooksCache.push_back((FuncHook*)funcHook);
	}

	template<typename Hook>
	static void RequestHook(std::string_view sig) {
		RequestHook<Hook>(MemoryUtil::findSignature(sig));
	}

	template<typename Hook>
	static void RequestHook(uintptr_t** VTable, int index) {
		if (VTable == nullptr)
			return;
		RequestHook<Hook>((uintptr_t)VTable[index]);
	}

	template<typename TRet>
	static TRet* getHook() {
		for (FuncHook* funcHook : hooksCache) {
			TRet* result = dynamic_cast<TRet*>(funcHook);
			if (result == nullptr)
				continue;
			return result;
		}
		return nullptr;
	}
};

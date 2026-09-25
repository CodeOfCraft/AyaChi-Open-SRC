#include "FuncHook.h"
#include <MinHook.h>
#include "../../../../Utils/Logger.h"

bool FuncHook::enableHook() {
	if (!enable) {
		if (!address) {
			LOGE("Hook %s has no target address", name);
			return false;
		}
		const MH_STATUS createStatus =
			MH_CreateHook((void*)address, func, reinterpret_cast<LPVOID*>(OriginFunc));
		if (createStatus != MH_OK) {
			LOGE("Failed to create hook %s at %p: %s", name,
				reinterpret_cast<void*>(address), MH_StatusToString(createStatus));
			return false;
		}
		const MH_STATUS enableStatus = MH_EnableHook((void*)address);
		if (enableStatus != MH_OK) {
			LOGE("Failed to enable hook %s at %p: %s", name,
				reinterpret_cast<void*>(address), MH_StatusToString(enableStatus));
			return false;
		}
		enable = true;
		return true;
	}
	return true;
}

void FuncHook::onHookRequest() {
}

#include "HookManager.h"
#include "Hooks/Options/GetGammaHook.h"
#include "Hooks/Options/GetHideHandHook.h"
#include "Hooks/Options/GetPerspectiveHook.h"
#include "Hooks/World/DimensionGetTimeOfDayHook.h"
#include "../../../SDK/Runtime/GameData.h"

void HookManager::registerOptionAndWorldHooks() {
	RequestHook<GetGammaHook>(Addresses::Options_getGamma);
	RequestHook<GetPerspectiveHook>(Addresses::Options_getPerspective);
	RequestHook<GetHideHandHook>(Addresses::Options_getHideHand);
	RequestHook<DimensionGetTimeOfDayHook>(Addresses::Dimension_getTimeOfDay);
}

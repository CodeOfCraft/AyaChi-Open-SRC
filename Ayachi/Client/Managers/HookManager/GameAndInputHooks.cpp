#include "HookManager.h"
#include "Hooks/Game/ClientInstanceUpdateHook.h"
#include "Hooks/Game/DisplayClientMessageHook.h"
#include "Hooks/Game/MinecraftGameUpdateHook.h"
#include "Hooks/Game/MultiPlayerLevelSubTickHook.h"
#include "Hooks/Game/SendChatMessageHook.h"
#include "Hooks/Input/ApplyTurnDeltaHook.h"
#include "Hooks/Input/GrabCursorHook.h"
#include "Hooks/Input/KeyMapHook.h"
#include "Hooks/Input/MouseInputHook.h"
#include "../../../SDK/Runtime/GameData.h"

void HookManager::registerGameAndInputHooks() {
	RequestHook<ClientInstanceUpdateHook>(Addresses::ClientInstance_update);
	RequestHook<MinecraftGameUpdateHook>(Addresses::MinecraftGame_update);
	RequestHook<MultiPlayerLevelSubTickHook>(Addresses::MultiPlayerLevel_subTick);
	RequestHook<SendChatMessageHook>(Addresses::ClientInstanceScreenModel_sendChatMessage);
	RequestHook<DisplayClientMessageHook>(Addresses::GuiData_displayClientMessage);
	RequestHook<MouseInputHook>(Addresses::GameCore_handleMouseInput);
	RequestHook<GrabCursorHook>(Addresses::ClientInstance_grabCursor);
	RequestHook<ApplyTurnDeltaHook>(Addresses::LocalPlayer_applyTurnDelta);
	RequestHook<KeyMapHook>(Addresses::KeyPressFunc);
}

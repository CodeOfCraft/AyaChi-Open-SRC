#include "GameData.h"

namespace GameData {
	uintptr_t Address::Platform_GameCore_winMain = 0;

	uintptr_t Address::KeyMap = 0;
	uintptr_t Address::GameCore_handleMouseInput = 0;
	uintptr_t Address::ClientInputUpdateSystemInternal_tickUpdateClientInput = 0;
	uintptr_t Address::KeyPressFunc = 0;
	uintptr_t Address::KeyMouseFunc = 0;

	uintptr_t Address::ClientInstance_update = 0;
	uintptr_t Address::ClientInstance_grabCursor = 0;
	uintptr_t Address::ClientInstance_releaseCursor = 0;
	uintptr_t Address::MinecraftGame_update = 0;
	uintptr_t Address::MinecraftGame_onDeviceLost = 0;
	uintptr_t Address::ClientInstanceScreenModel_sendChatMessage = 0;
	uintptr_t Address::ClientInstanceScreenModel_forwardSoundSubtitle = 0;
	uintptr_t Address::GuiData_displayClientMessage = 0;
	uintptr_t Address::AppPlatformGDK_releaseMouse = 0;
	uintptr_t Address::AppPlatform_GameCorePC_pickImage = 0;
	uintptr_t Address::MainWindow_windowProcCallback = 0;

	uintptr_t Address::Options_getGamma = 0;
	uintptr_t Address::Options_getPerspective = 0;
	uintptr_t Address::Options_setPerspective = 0;
	uintptr_t Address::Options_getHideHand = 0;

	uintptr_t Address::LevelRenderer_renderLevel = 0;
	uintptr_t Address::LevelRendererPlayer_getFov = 0;
	uintptr_t Address::LevelRendererPlayer_getFovForCameraReturn = 0;
	uintptr_t Address::LevelRendererPlayer_renderOutlineSelection = 0;
	uintptr_t Address::LevelRendererPlayer_renderHitSelect = 0;
	uintptr_t Address::ScreenView_setupAndRender = 0;
	uintptr_t Address::ItemInHandRenderer_renderFirstPerson = 0;
	uintptr_t Address::ItemInHandRenderer_renderOffhandItem = 0;
	uintptr_t Address::RenderController_getOverlayColor = 0;
	uintptr_t Address::BaseActorRenderer_renderText = 0;
	uintptr_t Address::Tessellator_begin = 0;
	uintptr_t Address::Tessellator_vertex = 0;
	uintptr_t Address::Tessellator_color = 0;
	uintptr_t Address::MeshHelpers_renderMeshImmediately = 0;
	uintptr_t Address::BaseActorRenderContext_Constructor = 0;
	uintptr_t Address::ItemRenderer_renderGuiItemNew = 0;
	uintptr_t Address::ActorRenderDispatcher_render = 0;
	uintptr_t Address::RenderMaterialGroup_common = 0;

	uintptr_t Address::LevelVTable = 0;
	uintptr_t Address::MultiPlayerLevel_subTick = 0;
	uintptr_t Address::Dimension_getTimeOfDay = 0;
	uintptr_t Address::Dimension_tick = 0;
	uintptr_t Address::Dimension_getSkyColor = 0;
	uintptr_t Address::Actor_attack = 0;
	uintptr_t Address::Actor_getNameTag = 0;
	uintptr_t Address::Actor_setNameTag = 0;
	uintptr_t Address::LocalPlayer_applyTurnDelta = 0;
	uintptr_t Address::UpdatePlayerFromCameraSystemUtil_updatePlayer = 0;
	uintptr_t Address::GuiMessageVector_emplaceBack = 0;

	uintptr_t Address::ItemStack_ItemStackBlock = 0;
	uintptr_t Address::ItemStackVtable = 0;
	uintptr_t Address::ItemStackBase_destructor = 0;
	uintptr_t Address::ItemStackBase_getHoverName = 0;
	uintptr_t Address::ItemStackBase_getDamageValue = 0;

	uintptr_t Address::MinecraftPackets_createPacket = 0;
	uintptr_t Address::RakPeer_GetAveragePing = 0;

	uintptr_t Address::MouseDevice_instance = 0;
	uintptr_t Address::ThirdPersonNametag = 0;
	uintptr_t Address::GpuInfo = 0;
	uintptr_t Address::MolangVariable_findOrAddVariableIndex = 0;
	uintptr_t Address::MolangVariableMap_getOrAddMolangVariable = 0;
	uintptr_t Address::GameArguments_onUri = 0;

	void init() {
	}
}

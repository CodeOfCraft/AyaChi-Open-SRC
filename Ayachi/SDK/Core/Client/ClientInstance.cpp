#include "ClientInstance.h"
#include "Platform_GameCore.h"
#include "../../Runtime/GameData.h"

ClientInstance* ClientInstance::instance = nullptr;

ClientInstance* ClientInstance::get() {
	if (instance == nullptr) {
		Platform_GameCore* platform = Platform_GameCore::get();
		if (platform == nullptr)
			return nullptr;

		MinecraftGame* mcGame = platform->minecraftGame;
		if (mcGame == nullptr)
			return nullptr;

		instance = mcGame->getPrimaryClientInstance();
	}
	return instance;
}

void ClientInstance::reset() {
	instance = nullptr;
}

void ClientInstance::tick() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::tick, void>(this);
}

bool ClientInstance::update(bool isInitFinished) {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::update, bool, bool>(this, isInitFinished);
}

BlockSource* ClientInstance::getRegion() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getRegion, BlockSource*>(this);
}

LocalPlayer* ClientInstance::getLocalPlayer() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getLocalPlayer, LocalPlayer*>(this);
}

Actor* ClientInstance::getCameraActor() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getCameraActor, Actor*>(this);
}

Actor* ClientInstance::getCameraTargetActor() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getCameraTargetActor, Actor*>(this);
}

Minecraft* ClientInstance::getServerData() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getServerData, Minecraft*>(this);
}

bool ClientInstance::isLeavingGame() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isLeavingGame, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isDestroyingGame() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isDestroyingGame, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isShuttingDown() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isShuttingDown, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isSplitScreenActive() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isSplitScreenActive, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isExitingLevel() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isExitingLevel, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isInBedScreen() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isInBedScreen, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isInDeathScreen() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isInDeathScreen, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isKeyboardEnabled() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isKeyboardEnabled, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::hasCommands() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::hasCommands, bool>(const_cast<ClientInstance*>(this));
}

int ClientInstance::getSplitScreenCount() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getSplitScreenCount, int>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isShowingLoadingScreen() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isShowingLoadingScreen, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isReadyToRender() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isReadyToRender, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::useController() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::useController, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::useTouchscreen() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::useTouchscreen, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::getMouseGrabbed() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getMouseGrabbed, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::currentInputModeIsMouseAndKeyboard() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::currentInputModeIsMouseAndKeyboard, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::allowPicking() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::allowPicking, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isShowingMenu() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isShowingMenu, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isShowingPauseScreen() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isShowingPauseScreen, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isShowingProgressScreen() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isShowingProgressScreen, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isShowingWorldProgressScreen() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isShowingWorldProgressScreen, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isShowingRealmsProgressScreen() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isShowingRealmsProgressScreen, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isShowingDeathScreen() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isShowingDeathScreen, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isShowingServerForm() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isShowingServerForm, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isScreenReplaceable() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isScreenReplaceable, bool>(const_cast<ClientInstance*>(this));
}

Level* ClientInstance::getLevel() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getLevel, Level*>(this);
}

bool ClientInstance::isWorldActive() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isWorldActive, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isInRealm() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isInRealm, bool>(this);
}

bool ClientInstance::readyForShutdown() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::readyForShutdown, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isPrimaryClient() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isPrimaryClient, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isEduMode() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isEduMode, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isGamepadCursorEnabled() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isGamepadCursorEnabled, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isInControlCustomization() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isInControlCustomization, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::hasLevel() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::hasLevel, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isPreGame() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isPreGame, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isInMultiplayerGame() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isInMultiplayerGame, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isMultiPlayerClient() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isMultiPlayerClient, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isInWorldAndNotShowingAnyMenuScreens() const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::ClientInstance::isInWorldAndNotShowingAnyMenuScreens,
		bool>(const_cast<ClientInstance*>(this));
}

MinecraftGame* ClientInstance::getMinecraftGame() {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::ClientInstance::getMinecraftGame,
		MinecraftGame*>(this);
}

void* ClientInstance::getOptions() {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::ClientInstance::getOptions,
		void*>(this);
}

GameRenderer* ClientInstance::getGameRenderer() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getGameRenderer, GameRenderer*>(this);
}

LevelRenderer* ClientInstance::getLevelRenderer() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getLevelRenderer, LevelRenderer*>(this);
}

bool ClientInstance::shouldRenderUICursor() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::shouldRenderUICursor, bool>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::getRenderPlayerModel() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getRenderPlayerModel, bool>(const_cast<ClientInstance*>(this));
}

float ClientInstance::getGuiScale() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getGuiScale, float>(const_cast<ClientInstance*>(this));
}

GuiData* ClientInstance::getGuiData() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getGuiData, GuiData*>(this);
}

PacketSender* ClientInstance::getPacketSender() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getPacketSender, PacketSender*>(this);
}

void* ClientInstance::getInput() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getInput, void*>(this);
}

int ClientInstance::getCurrentInputMode() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getCurrentInputMode, int>(const_cast<ClientInstance*>(this));
}

void* ClientInstance::getMinecraftInput() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getMinecraftInput, void*>(const_cast<ClientInstance*>(this));
}

Vec3<float> ClientInstance::getLastPointerLocation() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getLastPointerLocation, Vec3<float>>(this);
}

Vec2<float> ClientInstance::getTouchPointerLocation(int id) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getTouchPointerLocation, Vec2<float>, int>(
		const_cast<ClientInstance*>(this), id);
}

int ClientInstance::getActionPointerId() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getActionPointerId, int>(const_cast<ClientInstance*>(this));
}

int ClientInstance::getControllerId() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getControllerId, int>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::hasConnectedController() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::hasConnectedController, bool>(const_cast<ClientInstance*>(this));
}

void ClientInstance::refocusMouse(bool lostMouse) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::refocusMouse, void, bool>(this, lostMouse);
}

void* ClientInstance::getSoundEngine() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getSoundEngine, void*>(const_cast<ClientInstance*>(this));
}

void* ClientInstance::getMusicManager() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getMusicManager, void*>(const_cast<ClientInstance*>(this));
}

bool ClientInstance::isFullVanillaPackOnStack() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::isFullVanillaPackOnStack, bool>(const_cast<ClientInstance*>(this));
}

ItemInHandRenderer* ClientInstance::getItemInHandRenderer() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getItemInHandRenderer, ItemInHandRenderer*>(this);
}

ItemRenderer* ClientInstance::getItemRenderer() {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::ClientInstance::getItemRenderer,
		ItemRenderer*>(this);
}

void ClientInstance::grabMouse() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::grabMouse, void>(this);
}

void ClientInstance::releaseMouse() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::releaseMouse, void>(this);
}

void ClientInstance::grabCursor() {
	if (Addresses::ClientInstance_grabCursor != 0) {
		reinterpret_cast<void(__fastcall*)(void*)>(Addresses::ClientInstance_grabCursor)(this);
		return;
	}
	grabMouse();
}

void ClientInstance::releaseCursor() {
	if (Addresses::ClientInstance_releaseCursor != 0) {
		reinterpret_cast<void(__fastcall*)(void*)>(Addresses::ClientInstance_releaseCursor)(this);
		return;
	}
	releaseMouse();
}

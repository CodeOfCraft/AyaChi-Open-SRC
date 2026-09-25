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

bool ClientInstance::update(bool isInitFinished) {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::update, bool, bool>(this, isInitFinished);
}

BlockSource* ClientInstance::getRegion() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getRegion, BlockSource*>(this);
}

LocalPlayer* ClientInstance::getLocalPlayer() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getLocalPlayer, LocalPlayer*>(this);
}

bool ClientInstance::getMouseGrabbed() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getMouseGrabbed, bool>(this);
}

Level* ClientInstance::getLevel() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getLevel, Level*>(this);
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

IOptionRegistry* ClientInstance::getOptions() {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::ClientInstance::getOptions,
		IOptionRegistry*>(this);
}

GameRenderer* ClientInstance::getGameRenderer() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getGameRenderer, GameRenderer*>(this);
}

LevelRenderer* ClientInstance::getLevelRenderer() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getLevelRenderer, LevelRenderer*>(this);
}

GuiData* ClientInstance::getGuiData() {
	return *reinterpret_cast<GuiData**>(reinterpret_cast<uintptr_t>(this) + 0x648);
}

PacketSender* ClientInstance::getPacketSender() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getPacketSender, PacketSender*>(this);
}

ClientInputHandler* ClientInstance::getInput() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::ClientInstance::getInput, ClientInputHandler*>(this);
}

ItemRenderer* ClientInstance::getItemRenderer() {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::ClientInstance::getItemRenderer,
		ItemRenderer*>(this);
}

void ClientInstance::grabCursor() {
	if (Addresses::ClientInstance_grabCursor == 0)
		return;
	reinterpret_cast<void(__fastcall*)(void*)>(Addresses::ClientInstance_grabCursor)(this);
}

void ClientInstance::releaseCursor() {
	if (Addresses::ClientInstance_releaseCursor == 0)
		return;
	reinterpret_cast<void(__fastcall*)(void*)>(Addresses::ClientInstance_releaseCursor)(this);
}

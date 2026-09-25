#include "Level.h"

Actor* Level::fetchEntity(ActorUniqueID actorId, bool getRemoved) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::fetchEntity, Actor*, ActorUniqueID, bool>(
		const_cast<Level*>(this), actorId, getRemoved);
}

Actor* Level::getRuntimeEntity(ActorRuntimeID actorId, bool getRemoved) const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Level::getRuntimeEntity,
		Actor*,
		ActorRuntimeID,
		bool>(const_cast<Level*>(this), actorId, getRemoved);
}

Player* Level::getPlayer(const std::string& name) const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Level::getPlayerByName,
		Player*,
		const std::string&>(const_cast<Level*>(this), name);
}

Player* Level::getPlayer(ActorUniqueID actorId) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::getPlayerByUniqueId, Player*, ActorUniqueID>(
		const_cast<Level*>(this), actorId);
}

Player* Level::getPlayerByXuid(const std::string& xuid) const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Level::getPlayerByXuid,
		Player*,
		const std::string&>(const_cast<Level*>(this), xuid);
}

Player* Level::getPlatformPlayer(const std::string& platformOnlineId) const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Level::getPlatformPlayer,
		Player*,
		const std::string&>(const_cast<Level*>(this), platformOnlineId);
}

Player* Level::getPlayerFromServerId(const std::string& serverId) const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Level::getPlayerFromServerId,
		Player*,
		const std::string&>(const_cast<Level*>(this), serverId);
}

Player* Level::getRuntimePlayer(ActorRuntimeID runtimeId) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::getRuntimePlayer, Player*, ActorRuntimeID>(
		const_cast<Level*>(this), runtimeId);
}

int Level::getNumRemotePlayers() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::getNumRemotePlayers, int>(
		const_cast<Level*>(this));
}

Player* Level::getPrimaryLocalPlayer() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::getPrimaryLocalPlayer, Player*>(
		const_cast<Level*>(this));
}

void Level::tick() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Level::tick, void>(this);
}

int Level::getTime() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::getTime, int>(const_cast<Level*>(this));
}

void Level::setTime(int time) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Level::setTime, void, int>(this, time);
}

uint32_t Level::getSeed() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::getSeed, uint32_t>(this);
}

bool Level::isClientSide() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::isClientSide, bool>(const_cast<Level*>(this));
}

std::vector<Actor*> Level::getRuntimeActorList() const {
	std::vector<Actor*> actors;
	MemoryUtil::CallVFunc<
		GameData::VTableIndex::Level::getRuntimeActorList,
		void,
		std::vector<Actor*>&>(const_cast<Level*>(this), actors);
	return actors;
}

PacketSender* Level::getPacketSender() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::getPacketSender, PacketSender*>(
		const_cast<Level*>(this));
}

bool Level::hasStartWithMapEnabled() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::hasStartWithMapEnabled, bool>(
		const_cast<Level*>(this));
}

bool Level::isEditorWorld() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::isEditorWorld, bool>(const_cast<Level*>(this));
}

bool Level::getAllowAnonymousBlockDropsInEditorWorlds() const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Level::getAllowAnonymousBlockDropsInEditorWorlds,
		bool>(const_cast<Level*>(this));
}

bool Level::isHardcore() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::isHardcore, bool>(const_cast<Level*>(this));
}

bool Level::getTearingDown() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Level::getTearingDown, bool>(
		const_cast<Level*>(this));
}

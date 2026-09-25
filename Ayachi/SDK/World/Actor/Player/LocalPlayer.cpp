#include "LocalPlayer.h"
#include "../../../Runtime/GameData.h"

void LocalPlayer::openInventory() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::openInventory, void>(this);
}

bool LocalPlayer::isLoading() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::isLoading, bool>(
		const_cast<LocalPlayer*>(this));
}

void LocalPlayer::stopLoading() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::stopLoading, void>(this);
}

void LocalPlayer::respawn() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::respawn, void>(this);
}

void LocalPlayer::resetRot() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::resetRot, void>(this);
}

bool LocalPlayer::isAutoJumpEnabled() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::isAutoJumpEnabled, bool>(
		const_cast<LocalPlayer*>(this));
}

uint32_t LocalPlayer::getUserId() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::getUserId, uint32_t>(
		const_cast<LocalPlayer*>(this));
}

bool LocalPlayer::isInTrialMode() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::isInTrialMode, bool>(this);
}

void LocalPlayer::addExperience(int experience) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::addExperience, void, int>(this, experience);
}

void LocalPlayer::addLevels(int levels) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::addLevels, void, int>(this, levels);
}

bool LocalPlayer::isTeacher() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::isTeacher, bool>(
		const_cast<LocalPlayer*>(this));
}

void LocalPlayer::sendNetworkPacket(Packet& packet) const {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::sendNetworkPacket, void, Packet&>(
		const_cast<LocalPlayer*>(this), packet);
}

uint8_t LocalPlayer::getMaxChunkBuildRadius() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::LocalPlayer::getMaxChunkBuildRadius, uint8_t>(
		const_cast<LocalPlayer*>(this));
}

void LocalPlayer::_applyTurnDelta(const Vec2<float>& delta) {
	if (Addresses::LocalPlayer_applyTurnDelta == 0)
		return;

	reinterpret_cast<void(__fastcall*)(LocalPlayer*, const Vec2<float>&)>(Addresses::LocalPlayer_applyTurnDelta)(this, delta);
}

void LocalPlayer::applyTurnDelta(const Vec2<float>& delta) {
	_applyTurnDelta(delta);
}

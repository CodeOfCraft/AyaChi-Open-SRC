#include "LevelRendererPlayer.h"

Vec3<float>& LevelRendererPlayer::getOrigin() {
	return *reinterpret_cast<Vec3<float>*>(reinterpret_cast<uintptr_t>(this) + 0x660);
}

float& LevelRendererPlayer::getFovX() {
	return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x270);
}

float& LevelRendererPlayer::getFovY() {
	return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x274);
}

void LevelRendererPlayer::addCameraListenerToRenderChunkCoordinator() {
	MemoryUtil::CallVFunc<
		GameData::VTableIndex::LevelRendererPlayer::addCameraListenerToRenderChunkCoordinator,
		void>(this);
}

void LevelRendererPlayer::onAppSuspended() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::onAppSuspended, void>(this);
}

void LevelRendererPlayer::onAppResumed() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::onAppResumed, void>(this);
}

void LevelRendererPlayer::onDeviceLost() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::onDeviceLost, void>(this);
}

void LevelRendererPlayer::onLowMemory() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::onLowMemory, void>(this);
}

void LevelRendererPlayer::initResources() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::initResources, void>(this);
}

void LevelRendererPlayer::tickLevelRendererCamera() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::tickLevelRendererCamera, void>(this);
}

void LevelRendererPlayer::tickRain() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::tickRain, void>(this);
}

bool LevelRendererPlayer::getForceFog(const Actor& cameraActor) const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::LevelRendererPlayer::getForceFog,
		bool,
		const Actor&>(const_cast<LevelRendererPlayer*>(this), cameraActor);
}

float LevelRendererPlayer::getAmbientBrightness() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::getAmbientBrightness, float>(
		const_cast<LevelRendererPlayer*>(this));
}

void LevelRendererPlayer::recalculateRenderDistance(float renderDistanceScalar) {
	MemoryUtil::CallVFunc<
		GameData::VTableIndex::LevelRendererPlayer::recalculateRenderDistance,
		void,
		float>(this, renderDistanceScalar);
}

void LevelRendererPlayer::postRenderUpdate() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::postRenderUpdate, void>(this);
}

void LevelRendererPlayer::releaseRespectiveResources() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::releaseRespectiveResources, void>(this);
}

void LevelRendererPlayer::setupViewArea() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::LevelRendererPlayer::setupViewArea, void>(this);
}

float LevelRendererPlayer::getUnderwaterVisionClarity() {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::LevelRendererPlayer::getUnderwaterVisionClarity,
		float>(this);
}

void LevelRendererPlayer::renderHitSelect(
	BaseActorRenderContext& renderContext,
	BlockSource& blockSource,
	const BlockPos& position,
	bool fancyGraphics) const {
	if (GameData::Address::LevelRendererPlayer_renderHitSelect == 0)
		return;

	using Function = void(__fastcall*)(
		const LevelRendererPlayer*, BaseActorRenderContext&, BlockSource&, const BlockPos&, bool);
	reinterpret_cast<Function>(GameData::Address::LevelRendererPlayer_renderHitSelect)(
		this, renderContext, blockSource, position, fancyGraphics);
}

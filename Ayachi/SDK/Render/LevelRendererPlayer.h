#pragma once
#include "../../Utils/Maths.h"
#include "../../Utils/MemoryUtil.h"
#include "../World/Level/BlockSource.h"

class Actor;
class BaseActorRenderContext;

class LevelRendererPlayer {
public:
	Vec3<float>& getOrigin();
	float& getFovX();
	float& getFovY();

	void addCameraListenerToRenderChunkCoordinator();
	void onAppSuspended();
	void onAppResumed();
	void onDeviceLost();
	void onLowMemory();
	void initResources();
	void tickLevelRendererCamera();
	void tickRain();
	bool getForceFog(const Actor& cameraActor) const;
	float getAmbientBrightness() const;
	void recalculateRenderDistance(float renderDistanceScalar);
	void postRenderUpdate();
	void releaseRespectiveResources();
	void setupViewArea();
	float getUnderwaterVisionClarity();
	void renderHitSelect(
		BaseActorRenderContext& renderContext,
		BlockSource& blockSource,
		const BlockPos& position,
		bool fancyGraphics) const;
};

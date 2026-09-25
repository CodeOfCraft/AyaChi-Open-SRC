#include "BreakIndicators.h"

#include <algorithm>

#include "../../../../../../Renderer/ImGuiRender.h"
#include "../../../../../../SDK/World/Actor/Player/LocalPlayer.h"

BreakIndicators::BreakIndicators()
	: Module("BreakIndicators", "Render the progress of the block being broken", Category::RENDER) {
	registerSetting(new ColorSetting("StartColor", "Color at the start of breaking", &startColor, UIColor(255, 0, 0), false));
	registerSetting(new ColorSetting("EndColor", "Color near completion", &endColor, UIColor(0, 255, 0), false));
	registerSetting(new SliderSetting<int>("Alpha", "Fill opacity", &alpha, 40, 0, 255));
	registerSetting(new SliderSetting<int>("LineAlpha", "Outline opacity", &lineAlpha, 175, 0, 255));
}

void BreakIndicators::onImGuiRender(ImDrawList* drawlist) {
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	if (drawlist == nullptr || localPlayer == nullptr || localPlayer->gameMode == nullptr)
		return;

	GameMode* gameMode = localPlayer->gameMode;
	const float progress = std::clamp(gameMode->destroyProgress, 0.f, 1.f);
	if (progress <= 0.f)
		return;

	const Vec3<float> lower = gameMode->destroyBlockPos.CastTo<float>();
	const AABB box{ lower, lower.add(1.f) };
	const UIColor progressColor = ColorUtil::lerp(startColor, endColor, progress);
	ImGuiRender::drawBox3D(
		drawlist,
		box,
		UIColor(progressColor.r, progressColor.g, progressColor.b, static_cast<uint8_t>(alpha)),
		UIColor(progressColor.r, progressColor.g, progressColor.b, static_cast<uint8_t>(lineAlpha)),
		1.f,
		progress);
}

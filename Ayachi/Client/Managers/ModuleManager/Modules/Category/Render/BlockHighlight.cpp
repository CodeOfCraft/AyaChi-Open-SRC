#include "BlockHighlight.h"

#include "../../../../../../Renderer/ImGuiRender.h"
#include "../../../../../../SDK/World/Level/Block/Block.h"

BlockHighlight::BlockHighlight()
	: Module("BlockHighlight", "Highlight the block you are looking at", Category::RENDER) {
	registerSetting(new ColorSetting("Color", "Highlight color", &color, UIColor(255, 255, 255), false));
	registerSetting(new SliderSetting<int>("Alpha", "Fill opacity", &alpha, 40, 0, 255));
	registerSetting(new SliderSetting<int>("LineAlpha", "Outline opacity", &lineAlpha, 175, 0, 255));
	registerSetting(new BoolSetting("Slide", "Smoothly move between blocks", &slide, false));
	registerSetting(new SliderSetting<float>(
		"SlideSpeed", "Slide animation speed", &slideSpeed, 10.f, 5.f, 20.f,
		[this]() { return slide; }));
}

void BlockHighlight::onDisable() {
	std::scoped_lock lock(selectionMutex);
	renderInitialized = false;
	lastSelection = {};
}

void BlockHighlight::onRenderOutlineSelection(
	const Block& block,
	const BlockSource& blockSource,
	const BlockPos& position,
	bool& cancel) {
	BlockType* blockType = block.getBlockType();
	if (blockType == nullptr)
		return;

	AABB outline{};
	blockType->getOutline(block, blockSource, position, outline);
	std::scoped_lock lock(selectionMutex);
	targetAABB = outline;
	lastSelection = std::chrono::steady_clock::now();
	cancel = true;
}

void BlockHighlight::onImGuiRender(ImDrawList* drawlist) {
	if (drawlist == nullptr)
		return;

	AABB target{};
	std::chrono::steady_clock::time_point selectionTime;
	{
		std::scoped_lock lock(selectionMutex);
		target = targetAABB;
		selectionTime = lastSelection;
	}

	const auto now = std::chrono::steady_clock::now();
	if (selectionTime.time_since_epoch().count() == 0 ||
		now - selectionTime > std::chrono::milliseconds(150)) {
		renderInitialized = false;
		return;
	}

	if (!slide || !renderInitialized) {
		renderAABB = target;
		renderInitialized = true;
	} else {
		const float animationStep = std::clamp(ImGui::GetIO().DeltaTime * slideSpeed, 0.f, 1.f);
		renderAABB.lower = renderAABB.lower.lerp(
			target.lower, animationStep, animationStep, animationStep);
		renderAABB.upper = renderAABB.upper.lerp(
			target.upper, animationStep, animationStep, animationStep);
	}

	ImGuiRender::drawBox3D(
		drawlist,
		renderAABB,
		UIColor(color.r, color.g, color.b, static_cast<uint8_t>(alpha)),
		UIColor(color.r, color.g, color.b, static_cast<uint8_t>(lineAlpha)));
}

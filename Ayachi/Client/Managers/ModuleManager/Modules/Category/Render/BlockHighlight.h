#pragma once

#include <chrono>
#include <mutex>

#include "../../ModuleBase/Module.h"

class BlockHighlight : public Module {
private:
	UIColor color = UIColor(255, 255, 255);
	int alpha = 40;
	int lineAlpha = 175;
	bool slide = false;
	float slideSpeed = 10.f;

	std::mutex selectionMutex;
	AABB targetAABB{};
	AABB renderAABB{};
	std::chrono::steady_clock::time_point lastSelection{};
	bool renderInitialized = false;

public:
	BlockHighlight();

	void onDisable() override;
	void onRenderOutlineSelection(
		const Block& block,
		const BlockSource& blockSource,
		const BlockPos& position,
		bool& cancel) override;
	void onImGuiRender(ImDrawList* drawlist) override;
};

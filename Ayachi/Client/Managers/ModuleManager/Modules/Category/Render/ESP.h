#pragma once

#include <chrono>
#include <mutex>

#include "../../ModuleBase/Module.h"

class ESP : public Module {
private:
	UIColor color = UIColor(255, 255, 255);
	int alpha = 40;
	int lineAlpha = 135;
	bool mobs = false;
	bool items = false;
	std::mutex snapshotMutex;
	std::vector<AABB> boxSnapshot;
	std::chrono::steady_clock::time_point snapshotTime{};

public:
	ESP();
	void onDisable() override;
	void onLevelRender(LevelRenderer* levelRenderer, ScreenContext* screenContext) override;
	void onImGuiRender(ImDrawList* drawlist) override;
};

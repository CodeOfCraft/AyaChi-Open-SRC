#pragma once

#include "../../ModuleBase/Module.h"

class Arraylist : public Module {
private:
	float bgOpacity = 0.58f;
	bool rounded = true;
	float rounding = 4.f;
	float scale = 1.f;
	bool glow = true;
	float glowStrength = 0.8f;
	UIColor textColor = UIColor(255, 255, 255);
	bool bottom = false;

public:
	Arraylist();
	void onEnable() override;
	void onDisable() override;
	void onImGuiRender(ImDrawList* drawlist) override;
};

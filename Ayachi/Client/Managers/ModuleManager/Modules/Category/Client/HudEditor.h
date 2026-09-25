#pragma once
#include "../../ModuleBase/Module.h"
#include "../../../../InputManager/MouseInput.h"
#include <unordered_map>

struct HudElement {
	std::string id;
	std::string label;
	Vec2<float> position;
	Vec2<float> defSize;
	Vec2<float> actualSize;
	bool visible;
	bool customPosition;
};

class HudEditor : public Module {
private:
	int draggingIndex = -1;
	Vec2<float> dragOffset;

	bool snappedH = false, snappedV = false;
	float snapLineY = 0.f;
	float snapLineX = 0.f;

	bool wasClickGUIOpen = false;

	static inline std::vector<HudElement> elements;
public:
	HudEditor();

	void onEnable() override;
	void onDisable() override;
	void onImGuiRender(ImDrawList* drawlist) override;
	void onLoadConfig(void* conf) override;
	void onSaveConfig(void* conf) override;
	void onKeyUpdate(int key, bool isDown) override;

	static Vec2<float> getPosition(const std::string& elementId);

	static bool hasCustomPosition(const std::string& elementId);

	static void setVisible(const std::string& elementId, bool vis);

	static void setActualSize(const std::string& elementId, Vec2<float> size);

	static void setDefaultPosition(const std::string& elementId, Vec2<float> pixelPos, Vec2<float> screenSize);
};

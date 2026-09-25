#pragma once
#include <optional>
#include <string>
#include <vector>
#include "../../../Utils/Maths.h"
#include "../../../Utils/MemoryUtil.h"
#include "GuiMessage.h"

class GuiData {
public:
	CLASS_MEMBER(Vec2<float>, screenSize, 0x40);
	CLASS_MEMBER(Vec2<float>, guiSize, 0x50);
	CLASS_MEMBER(float, guiScale, 0x5C);
	CLASS_MEMBER(float, guiScaleFrac, 0x60);
	CLASS_MEMBER(std::vector<GuiMessage>, messages, 0x150);

	void updateGuiScale(float newScale) {
		guiScale = newScale;
		guiScaleFrac = 1.f / newScale;
		guiSize = Vec2<float>(screenSize.x * guiScaleFrac, screenSize.y * guiScaleFrac);
	}

	void displayClientMessage(const std::string& message, std::optional<std::string> secondary = std::nullopt, bool forceVisible = false) {
		if (Addresses::GuiData_displayClientMessage == 0)
			return;

		using func_t = void(*)(GuiData*, const std::string&, std::optional<std::string>, bool);
		reinterpret_cast<func_t>(Addresses::GuiData_displayClientMessage)(this, message, std::move(secondary), forceVisible);
	}
};

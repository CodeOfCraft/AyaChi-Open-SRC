#pragma once
#include <vector>
#include "Setting.h"
#include "SliderSetting.h"
#include "../../../../../../Utils/ColorUtil.h"

class ColorSetting : public Setting {
public:
	UIColor* colorPtr;
	std::vector<SliderSetting<uint8_t>*> colorSliders;
	bool colorSynced = false;
	bool showSynced = true;
	bool extended = false;
	float syncSelectedAnim = 0.f;
	ColorSetting(std::string settingName, std::string des, UIColor* ptr, UIColor defaultValue, bool alpha = true, bool _showSynced = true, std::optional<std::function<bool(void)>> _dependOn = std::nullopt) {
		this->name = settingName;
		this->description = des;
		this->colorPtr = ptr;
		this->showSynced = _showSynced;

		colorSliders.push_back(new SliderSetting<uint8_t>("Red", "NULL", &colorPtr->r, defaultValue.r, 0, 255));
		colorSliders.push_back(new SliderSetting<uint8_t>("Green", "NULL", &colorPtr->g, defaultValue.g, 0, 255));
		colorSliders.push_back(new SliderSetting<uint8_t>("Blue", "NULL", &colorPtr->b, defaultValue.b, 0, 255));

		if (alpha) colorSliders.push_back(new SliderSetting<uint8_t>("Alpha", "NULL", &colorPtr->a, defaultValue.a, 0, 255));
		this->dependOn = _dependOn;
		this->type = SettingType::COLOR_S;
	}

	void toJson(nlohmann::json& obj) const override {
		obj[name]["Red"] = colorPtr->r;
		obj[name]["Green"] = colorPtr->g;
		obj[name]["Blue"] = colorPtr->b;
		obj[name]["Alpha"] = colorPtr->a;
		obj[name]["Synced"] = colorSynced;
	}

	void fromJson(const nlohmann::json& obj) override {
		if (!obj.contains(name)) return;
		const nlohmann::json& c = obj[name];
		if (c.contains("Red") && c["Red"].is_number()) colorPtr->r = (uint8_t)c["Red"].get<int>();
		if (c.contains("Green") && c["Green"].is_number()) colorPtr->g = (uint8_t)c["Green"].get<int>();
		if (c.contains("Blue") && c["Blue"].is_number()) colorPtr->b = (uint8_t)c["Blue"].get<int>();
		if (c.contains("Alpha") && c["Alpha"].is_number()) colorPtr->a = (uint8_t)c["Alpha"].get<int>();
		if (c.contains("Synced") && c["Synced"].is_boolean()) colorSynced = c["Synced"].get<bool>();
	}

	~ColorSetting() {
		for (auto& slider : colorSliders) {
			delete slider;
			slider = nullptr;
		}
		colorSliders.clear();
	}
};

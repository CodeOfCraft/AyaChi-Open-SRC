#pragma once

#include "EnumSetting.h"
#include "../../../../FontManager/FontManager.h"

class FontSetting : public EnumSetting {
private:
	int appliedIndex;

	void refreshOptions() {
		enumList = FontManager::getFontOptions();
	}

public:
	FontSetting(std::string settingName, std::string des, int* ptr)
		: EnumSetting(std::move(settingName), std::move(des), FontManager::getFontOptions(), ptr,
			FontManager::getConfiguredFontIndex()),
		  appliedIndex(*ptr) {}

	void update() override {
		if (*value != appliedIndex) {
			if (!FontManager::setFontByIndex(*value))
				*value = FontManager::getConfiguredFontIndex();
			appliedIndex = *value;
			refreshOptions();
		}
	}

	void toJson(nlohmann::json& obj) const override {
		obj[name] = FontManager::getConfiguredFont();
	}

	void fromJson(const nlohmann::json& obj) override {
		if (!obj.contains(name))
			return;

		if (obj[name].is_string())
			FontManager::setFont(obj[name].get<std::string>());
		else if (obj[name].is_number_integer())
			FontManager::setFontByIndex(obj[name].get<int>());

		*value = FontManager::getConfiguredFontIndex();
		appliedIndex = *value;
		refreshOptions();
	}
};

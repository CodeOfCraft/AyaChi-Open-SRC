#pragma once
#include "Setting.h"
#include <vector>

class EnumSetting : public Setting {
public:
	std::vector<std::string> enumList;
	int* value;

	EnumSetting(std::string settingName, std::string des, std::vector<std::string> cEnumList, int* ptr, int defaultValue, std::optional<std::function<bool(void)>> _dependOn = std::nullopt) {
		this->name = settingName;
		this->description = des;
		this->enumList = cEnumList;
		this->value = ptr;
		*this->value = defaultValue;
		this->dependOn = _dependOn;
		this->type = SettingType::ENUM_S;
	}

	std::string getDisplayValue(std::size_t index) const {
		if (index >= enumList.size())
			return {};
		return enumList[index];
	}

	void toJson(nlohmann::json& obj) const override {
		obj[name] = *value;
	}

	void fromJson(const nlohmann::json& obj) override {
		if (obj.contains(name) && obj[name].is_number_integer()) {
			int v = obj[name].get<int>();
			if (!enumList.empty()) {
				if (v < 0) v = 0;
				if (v >= (int)enumList.size()) v = (int)enumList.size() - 1;
			}
			*value = v;
		}
	}
};

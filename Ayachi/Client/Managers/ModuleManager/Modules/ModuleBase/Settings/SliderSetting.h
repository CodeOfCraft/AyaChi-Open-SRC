#pragma once
#include "Setting.h"
#include <type_traits>

enum class ValueType {
	INT_T,
	FLOAT_T,
	UNKNOW_T
};

class SliderSettingBase : public Setting {
public:
	ValueType valueType = ValueType::UNKNOW_T;
};

template<typename T>
class SliderSetting : public SliderSettingBase {
public:
	T* valuePtr;
	T minValue;
	T maxValue;

	SliderSetting(std::string settingName, std::string des, T* vPtr, T defaultValue, T vMin, T vMax, std::optional<std::function<bool(void)>> _dependOn = std::nullopt) {
		this->name = settingName;
		this->description = des;
		this->valuePtr = vPtr;
		*this->valuePtr = defaultValue;
		this->minValue = vMin;
		this->maxValue = vMax;
		this->dependOn = _dependOn;
		this->type = SettingType::SLIDER_S;

		if constexpr (std::is_same_v<T, int>) {
			this->valueType = ValueType::INT_T;
		}
		else if constexpr (std::is_same_v<T, float>) {
			this->valueType = ValueType::FLOAT_T;
		}
		else {
			this->valueType = ValueType::UNKNOW_T;
		}
	}

	void toJson(nlohmann::json& obj) const override {
		obj[name] = *valuePtr;
	}

	void fromJson(const nlohmann::json& obj) override {
		if (obj.contains(name) && obj[name].is_number()) {
			T v = obj[name].get<T>();
			if (v < minValue) v = minValue;
			if (v > maxValue) v = maxValue;
			*valuePtr = v;
		}
	}
};
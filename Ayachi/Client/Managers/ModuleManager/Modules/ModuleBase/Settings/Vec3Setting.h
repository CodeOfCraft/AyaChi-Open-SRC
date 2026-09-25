#pragma once
#include "Setting.h"

enum class Vec3Type : uint8_t {
	UNKNOWN_T,
	FLOAT_T,
	INT_T
};

class Vec3SettingBase : public Setting {
public:
	Vec3Type valueType = Vec3Type::UNKNOWN_T;
	bool extended = false;
};

template<typename T>
class Vec3Setting : public Vec3SettingBase {
public:
	std::vector<SliderSetting<T>*> vecSliders;
public:
	Vec3<T>* valuePtr;
	Vec3<T> minValue;
	Vec3<T> maxValue;
public:
	Vec3Setting(std::string settingName, std::string des, Vec3<T>* ptr, Vec3<T> defaultValue, Vec3<T> minValue, Vec3<T> maxValue, std::optional<std::function<bool(void)>> _dependOn = std::nullopt) {
		this->name = settingName;
		this->description = des;
		this->valuePtr = ptr;
		*this->valuePtr = defaultValue;
		this->minValue = minValue;
		this->maxValue = maxValue;
	
		vecSliders.push_back(new SliderSetting<T>("X", "NULL", &valuePtr->x, defaultValue.x, minValue.x, maxValue.x));
		vecSliders.push_back(new SliderSetting<T>("Y", "NULL", &valuePtr->y, defaultValue.y, minValue.y, maxValue.y));
		vecSliders.push_back(new SliderSetting<T>("Z", "NULL", &valuePtr->z, defaultValue.z, minValue.z, maxValue.z));

		this->dependOn = _dependOn;
		this->type = SettingType::VEC3_S;

		if constexpr (std::is_same_v<T, int>) this->valueType = Vec3Type::INT_T;
		else if constexpr (std::is_same_v<T, float>) this->valueType = Vec3Type::FLOAT_T;
		else this->valueType = Vec3Type::UNKNOWN_T;
	}

	void toJson(nlohmann::json& obj) const override {
		obj[name]["X"] = valuePtr->x;
		obj[name]["Y"] = valuePtr->y;
		obj[name]["Z"] = valuePtr->z;
	}

	void fromJson(const nlohmann::json& obj) override {
		if (!obj.contains(name)) return;
		const nlohmann::json& v = obj[name];
		if (v.contains("X") && v["X"].is_number()) valuePtr->x = v["X"].get<T>();
		if (v.contains("Y") && v["Y"].is_number()) valuePtr->y = v["Y"].get<T>();
		if (v.contains("Z") && v["Z"].is_number()) valuePtr->z = v["Z"].get<T>();
	}

	~Vec3Setting() {
		for (auto& slider : vecSliders) {
			delete slider;
			slider = nullptr;
		}
		vecSliders.clear();
	}
};
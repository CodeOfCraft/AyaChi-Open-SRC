#pragma once
#include <string>
#include <optional>
#include <functional>
#include "../../../../../../Libs/json.hpp"

class Module;

enum class SettingType {
	UNKNOW_S,
	BOOL_S,
	KEYBIND_S,
	ENUM_S,
	COLOR_S,
	SLIDER_S,
	VEC3_S
};

class Setting {
public:
	std::string name;
	std::string description;
	std::optional<std::function<bool(void)>> dependOn = std::nullopt;
	SettingType type = SettingType::UNKNOW_S;
public:
	float selectedAnim = 0.f;
	bool isDragging = false;
	bool isVisible() const {
		if (dependOn.has_value()) {
			return dependOn.value()();
		}
		return true;
	}

	virtual ~Setting() = default;

	std::string getDisplayName() const {
		return name;
	}

	std::string getDescription() const {
		return description;
	}

	virtual void toJson(nlohmann::json& obj) const {}
	virtual void fromJson(const nlohmann::json& obj) {}
	virtual void update() {}
};

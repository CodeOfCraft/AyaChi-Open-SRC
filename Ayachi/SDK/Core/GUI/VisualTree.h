#pragma once

#include <cstdint>
#include <string>

#include "UIControl.h"

class VisualTree {
public:
	[[nodiscard]] UIControl* getRootControl() const {
		return *reinterpret_cast<UIControl* const*>(reinterpret_cast<uintptr_t>(this) + 0x8);
	}

	[[nodiscard]] const std::string& getRootControlName() const {
		return *reinterpret_cast<const std::string*>(reinterpret_cast<uintptr_t>(this) + 0x28);
	}
};

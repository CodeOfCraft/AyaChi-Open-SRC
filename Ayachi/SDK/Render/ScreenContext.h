#pragma once

#include <cstddef>

#include "Camera.h"

class ScreenContext {
public:
	[[nodiscard]] mce::Camera* getCamera() const noexcept {
		return *reinterpret_cast<mce::Camera* const*>(
			reinterpret_cast<const std::byte*>(this) + 0x18);
	}
};

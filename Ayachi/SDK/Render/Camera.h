#pragma once

#include <cstddef>

#include "MatrixStack.h"

namespace mce {
	class Camera {
	public:
		[[nodiscard]] MatrixStack* getViewMatrixStack() noexcept {
			return reinterpret_cast<MatrixStack*>(reinterpret_cast<std::byte*>(this) + 0x0);
		}

		[[nodiscard]] MatrixStack* getWorldMatrixStack() noexcept {
			return reinterpret_cast<MatrixStack*>(reinterpret_cast<std::byte*>(this) + 0x40);
		}

		[[nodiscard]] MatrixStack* getProjectionMatrixStack() noexcept {
			return reinterpret_cast<MatrixStack*>(reinterpret_cast<std::byte*>(this) + 0x80);
		}
	};
}

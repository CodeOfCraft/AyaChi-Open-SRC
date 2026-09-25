#pragma once

#include <cstddef>

#include "MatrixStack.h"

static_assert(sizeof(glm::vec3) == 0xC);

namespace mce {
	class Camera {
	public:
		[[nodiscard]] MatrixStack* getViewMatrixStack() noexcept {
			return reinterpret_cast<MatrixStack*>(reinterpret_cast<std::byte*>(this) + 0x0);
		}
		[[nodiscard]] const MatrixStack* getViewMatrixStack() const noexcept {
			return field<MatrixStack>(0x0);
		}

		[[nodiscard]] MatrixStack* getWorldMatrixStack() noexcept {
			return reinterpret_cast<MatrixStack*>(reinterpret_cast<std::byte*>(this) + 0x40);
		}
		[[nodiscard]] const MatrixStack* getWorldMatrixStack() const noexcept {
			return field<MatrixStack>(0x40);
		}

		[[nodiscard]] MatrixStack* getProjectionMatrixStack() noexcept {
			return reinterpret_cast<MatrixStack*>(reinterpret_cast<std::byte*>(this) + 0x80);
		}
		[[nodiscard]] const MatrixStack* getProjectionMatrixStack() const noexcept {
			return field<MatrixStack>(0x80);
		}

		[[nodiscard]] glm::mat4x4 getProjectionMatrix() const {
			return getProjectionMatrixStack()->top()._m;
		}

		[[nodiscard]] const glm::mat4x4& getInverseViewMatrix() const noexcept {
			return *field<glm::mat4x4>(0xC0);
		}
		[[nodiscard]] const glm::vec3& getRight() const noexcept { return *field<glm::vec3>(0x100); }
		[[nodiscard]] const glm::vec3& getUp() const noexcept { return *field<glm::vec3>(0x10C); }
		[[nodiscard]] const glm::vec3& getForward() const noexcept { return *field<glm::vec3>(0x118); }
		[[nodiscard]] const glm::vec3& getPosition() const noexcept { return *field<glm::vec3>(0x124); }
		[[nodiscard]] float getAspectRatio() const noexcept { return *field<float>(0x130); }
		[[nodiscard]] float getFov() const noexcept { return *field<float>(0x134); }
		[[nodiscard]] float getZNear() const noexcept { return *field<float>(0x138); }
		[[nodiscard]] float getZFar() const noexcept { return *field<float>(0x13C); }

	private:
		template <typename T>
		[[nodiscard]] const T* field(std::size_t offset) const noexcept {
			return reinterpret_cast<const T*>(reinterpret_cast<const std::byte*>(this) + offset);
		}
	};
}

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "../../../Utils/Maths.h"

class UIControl {
public:
	[[nodiscard]] Vec2<float>& getPosition() {
		return field<Vec2<float>>(0x0);
	}

	[[nodiscard]] const Vec2<float>& getPosition() const {
		return field<Vec2<float>>(0x0);
	}

	[[nodiscard]] const std::string& getName() const {
		return field<std::string>(0x10);
	}

	[[nodiscard]] Vec2<float>& getParentRelativePosition() {
		return field<Vec2<float>>(0x30);
	}

	[[nodiscard]] const Vec2<float>& getParentRelativePosition() const {
		return field<Vec2<float>>(0x30);
	}

	[[nodiscard]] Vec2<float>& getSize() {
		return field<Vec2<float>>(0x38);
	}

	[[nodiscard]] const Vec2<float>& getSize() const {
		return field<Vec2<float>>(0x38);
	}

	[[nodiscard]] float& getAlpha() {
		return field<float>(0x50);
	}

	[[nodiscard]] float getAlpha() const {
		return field<float>(0x50);
	}

	[[nodiscard]] int& getZOrder() {
		return field<int>(0x54);
	}

	[[nodiscard]] int getZOrder() const {
		return field<int>(0x54);
	}

	[[nodiscard]] std::vector<std::shared_ptr<UIControl>>& getChildren() {
		return field<std::vector<std::shared_ptr<UIControl>>>(0x88);
	}

	[[nodiscard]] const std::vector<std::shared_ptr<UIControl>>& getChildren() const {
		return field<std::vector<std::shared_ptr<UIControl>>>(0x88);
	}

	[[nodiscard]] UIControl* findChild(std::string_view name) const {
		for (const auto& child : getChildren()) {
			if (child != nullptr && child->getName() == name)
				return child.get();
		}
		return nullptr;
	}

	[[nodiscard]] UIControl* findFirstDescendant(std::string_view name) const {
		if (getName() == name)
			return const_cast<UIControl*>(this);

		for (const auto& child : getChildren()) {
			if (child == nullptr)
				continue;
			if (UIControl* match = child->findFirstDescendant(name); match != nullptr)
				return match;
		}
		return nullptr;
	}

private:
	template <typename T>
	[[nodiscard]] T& field(size_t offset) {
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	template <typename T>
	[[nodiscard]] const T& field(size_t offset) const {
		return *reinterpret_cast<const T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}
};

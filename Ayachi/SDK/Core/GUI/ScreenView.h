#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "VisualTree.h"

class ScreenController;

class ScreenView {
public:
	[[nodiscard]] float getDeltaTime() const {
		return *reinterpret_cast<const float*>(reinterpret_cast<uintptr_t>(this) + 0x4);
	}

	[[nodiscard]] const Vec2<float>& getSize() const {
		return *reinterpret_cast<const Vec2<float>*>(reinterpret_cast<uintptr_t>(this) + 0x10);
	}

	[[nodiscard]] ScreenController* getScreenController() const {
		return *reinterpret_cast<ScreenController* const*>(reinterpret_cast<uintptr_t>(this) + 0x38);
	}

	[[nodiscard]] VisualTree* getVisualTree() const {
		return *reinterpret_cast<VisualTree* const*>(reinterpret_cast<uintptr_t>(this) + 0x48);
	}

	[[nodiscard]] UIControl* getRootControl() const {
		VisualTree* tree = getVisualTree();
		return tree != nullptr ? tree->getRootControl() : nullptr;
	}

	[[nodiscard]] const std::string& getRootControlName() const {
		static const std::string empty;
		UIControl* root = getRootControl();
		return root != nullptr ? root->getName() : empty;
	}

	[[nodiscard]] bool isScreen(std::string_view name) const {
		return getRootControlName() == name;
	}

	[[nodiscard]] UIControl* findControl(std::string_view name) const {
		UIControl* root = getRootControl();
		return root != nullptr ? root->findFirstDescendant(name) : nullptr;
	}
};

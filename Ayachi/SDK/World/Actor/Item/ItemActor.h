#pragma once

#include <cstddef>

#include "../Actor.h"
#include "../../Item/ItemStack.h"

class ItemActor : public Actor {
public:
	ItemStack& getItem() noexcept {
		return *reinterpret_cast<ItemStack*>(
			reinterpret_cast<std::byte*>(this) + Actor::NativeSize);
	}

	const ItemStack& getItem() const noexcept {
		return *reinterpret_cast<const ItemStack*>(
			reinterpret_cast<const std::byte*>(this) + Actor::NativeSize);
	}
};

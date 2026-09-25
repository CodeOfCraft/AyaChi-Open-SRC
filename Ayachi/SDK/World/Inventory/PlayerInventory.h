#pragma once

#include <cstdint>
#include "Inventory.h"

class PlayerInventory {
public:
	PlayerInventory() = delete;

	int getSelectedSlot() const noexcept {
		return *reinterpret_cast<const int*>(reinterpret_cast<uintptr_t>(this) + 0x10);
	}

	Inventory* getInventory() const noexcept {
		return *reinterpret_cast<Inventory* const*>(reinterpret_cast<uintptr_t>(this) + 0xB8);
	}

	[[nodiscard]] ItemStack* getSelectedItem() {
		Inventory* inventory = getInventory();
		if (inventory == nullptr)
			return nullptr;

		const int slot = getSelectedSlot();
		if (slot < 0 || slot > 8 || slot >= inventory->getContainerSize())
			return nullptr;

		return inventory->getItem(slot);
	}

	[[nodiscard]] const ItemStack* getSelectedItem() const {
		return const_cast<PlayerInventory*>(this)->getSelectedItem();
	}
};

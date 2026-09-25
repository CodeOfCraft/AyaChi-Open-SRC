#pragma once

#include <cstdint>

class Inventory;

class PlayerInventory {
public:
	PlayerInventory() = delete;

	int getSelectedSlot() const noexcept {
		return *reinterpret_cast<const int*>(reinterpret_cast<uintptr_t>(this) + 0x10);
	}

	Inventory* getInventory() const noexcept {
		return *reinterpret_cast<Inventory* const*>(reinterpret_cast<uintptr_t>(this) + 0xB8);
	}
};

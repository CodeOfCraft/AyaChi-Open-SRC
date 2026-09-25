#pragma once

#include "../../Runtime/GameData.h"
#include "../../../Utils/MemoryUtil.h"

class ItemStack;

class Inventory {
public:
	Inventory() = delete;

	[[nodiscard]] ItemStack* getItem(int slot) {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Inventory::getItem, ItemStack*, int>(
			this, slot);
	}

	[[nodiscard]] const ItemStack* getItem(int slot) const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Inventory::getItem, const ItemStack*, int>(
			const_cast<Inventory*>(this), slot);
	}

	[[nodiscard]] int getContainerSize() const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Inventory::getContainerSize, int>(
			const_cast<Inventory*>(this));
	}

	[[nodiscard]] int getMaxStackSize() const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Inventory::getMaxStackSize, int>(
			const_cast<Inventory*>(this));
	}

	void setItem(int slot, const ItemStack& item) {
		MemoryUtil::CallVFunc<GameData::VTableIndex::Inventory::setItem, void, int, const ItemStack&>(
			this, slot, item);
	}

	void removeItem(int slot, int count) {
		MemoryUtil::CallVFunc<GameData::VTableIndex::Inventory::removeItem, void, int, int>(
			this, slot, count);
	}
};

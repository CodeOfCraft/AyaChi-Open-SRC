#pragma once

#include "../../Runtime/GameData.h"
#include "../../../Utils/MemoryUtil.h"

class ItemStack;

class Inventory {
public:
	Inventory() = delete;

	ItemStack* getItem(int slot) {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Inventory::getItem, ItemStack*, int>(
			this, slot);
	}
};

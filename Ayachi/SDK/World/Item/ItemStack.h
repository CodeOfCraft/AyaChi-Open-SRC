#pragma once

#include "ItemStackBase.h"
#include "ItemStackNetIdVariant.h"

class ItemStack : public ItemStackBase {
public:
	static ItemStack* constructFromBlock(
		void* storage,
		const Block& block,
		int count = 1,
		const CompoundTag* userData = nullptr);

	void destruct() noexcept;

	ItemStackNetIdVariant& getNetIdVariant() noexcept {
		return mNetIdVariant;
	}

	const ItemStackNetIdVariant& getNetIdVariant() const noexcept {
		return mNetIdVariant;
	}

private:
	ItemStackNetIdVariant mNetIdVariant;
};

static_assert(sizeof(ItemStack) == 0x98);
static_assert(alignof(ItemStack) == 8);

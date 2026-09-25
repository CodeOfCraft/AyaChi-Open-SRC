#include "../../../pch.h"

#include "ItemStack.h"

#include <cstring>

#include "../../Runtime/GameData.h"

ItemStack* ItemStack::constructFromBlock(
	void* storage,
	const Block& block,
	int count,
	const CompoundTag* userData) {
	if (storage == nullptr || GameData::Address::ItemStack_ItemStackBlock == 0 ||
		GameData::Address::ItemStackVtable == 0)
		return nullptr;

	std::memset(storage, 0, sizeof(ItemStack));

	using Function = ItemStack*(__fastcall*)(void*, const Block*, int, const CompoundTag*);
	auto* item = reinterpret_cast<Function>(GameData::Address::ItemStack_ItemStackBlock)(
		storage, &block, count, userData);
	if (item == nullptr)
		return nullptr;

	item->field<void**>(0x00) = reinterpret_cast<void**>(GameData::Address::ItemStackVtable);
	return item;
}

void ItemStack::destruct() noexcept {
	if (GameData::Address::ItemStackBase_destructor == 0)
		return;

	using Function = void(__fastcall*)(ItemStackBase*);
	reinterpret_cast<Function>(GameData::Address::ItemStackBase_destructor)(this);
}

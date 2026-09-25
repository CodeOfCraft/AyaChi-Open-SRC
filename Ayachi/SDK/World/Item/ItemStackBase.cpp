#include "../../../pch.h"

#include "ItemStackBase.h"

#include "Item.h"
#include "../../Runtime/GameData.h"

Item* ItemStackBase::getItem() const noexcept {
	const auto counter = field<uintptr_t>(0x08);
	return counter == 0 ? nullptr : *reinterpret_cast<Item* const*>(counter);
}

CompoundTag* ItemStackBase::getUserData() const noexcept {
	return field<CompoundTag*>(0x10);
}

const Block* ItemStackBase::getBlock() const noexcept {
	return field<const Block*>(0x18);
}

int16_t ItemStackBase::getAuxValue() const noexcept {
	return field<int16_t>(0x20);
}

uint8_t ItemStackBase::getCount() const noexcept {
	return field<uint8_t>(0x22);
}

bool ItemStackBase::isValid() const noexcept {
	return field<bool>(0x23);
}

void ItemStackBase::setCount(uint8_t count) noexcept {
	field<uint8_t>(0x22) = count;
}

std::string ItemStackBase::getHoverName() const {
	if (GameData::Address::ItemStackBase_getHoverName == 0)
		return {};

	std::string result;
	using Function = std::string*(__fastcall*)(const ItemStackBase*, std::string*);
	reinterpret_cast<Function>(GameData::Address::ItemStackBase_getHoverName)(this, &result);
	return result;
}

int16_t ItemStackBase::getDamageValue() const {
	if (GameData::Address::ItemStackBase_getDamageValue == 0)
		return 0;

	using Function = int16_t(__fastcall*)(const ItemStackBase*);
	return reinterpret_cast<Function>(GameData::Address::ItemStackBase_getDamageValue)(this);
}

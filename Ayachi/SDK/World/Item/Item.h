#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "../../Core/Types/HashedString.h"
#include "../../Runtime/GameData.h"
#include "../../../Utils/MemoryUtil.h"

class Block;
class ItemStack;
class ItemStackBase;

class Item {
public:
	Item() = delete;

	const std::string& getTextureAtlasFile() const noexcept {
		return field<std::string>(0x10);
	}

	uint8_t getMaxStackSize() const noexcept {
		return field<uint8_t>(0xA8);
	}

	int16_t getId() const noexcept {
		return field<int16_t>(0xAA);
	}

	const std::string& getDescriptionId() const noexcept {
		return field<std::string>(0xB0);
	}

	const HashedString& getRawNameId() const noexcept {
		return field<HashedString>(0xD0);
	}

	const std::string& getNamespace() const noexcept {
		return field<std::string>(0x100);
	}

	const HashedString& getFullName() const noexcept {
		return field<HashedString>(0x120);
	}

	int getMaxUseDuration(const ItemStack* item) const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Item::getMaxUseDuration, int, const ItemStack*>(
			const_cast<Item*>(this), item);
	}

	bool canDestroySpecial(const Block* block) const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Item::canDestroySpecial, bool, const Block*>(
			const_cast<Item*>(this), block);
	}

	int16_t getMaxDamage() const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Item::getMaxDamage, int16_t>(
			const_cast<Item*>(this));
	}

	bool isGlint(const ItemStackBase* item) const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Item::isGlint, bool, const ItemStackBase*>(
			const_cast<Item*>(this), item);
	}

	float getDestroySpeed(const ItemStackBase* item, const Block* block) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::Item::getDestroySpeed,
			float,
			const ItemStackBase*,
			const Block*>(const_cast<Item*>(this), item, block);
	}

private:
	template <typename T>
	const T& field(std::size_t offset) const noexcept {
		return *reinterpret_cast<const T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}
};

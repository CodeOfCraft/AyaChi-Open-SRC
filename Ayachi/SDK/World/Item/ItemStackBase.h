#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

class Block;
class CompoundTag;
class Item;

class alignas(8) ItemStackBase {
public:
	ItemStackBase() = delete;

	Item* getItem() const noexcept;
	CompoundTag* getUserData() const noexcept;
	const Block* getBlock() const noexcept;
	int16_t getAuxValue() const noexcept;
	uint8_t getCount() const noexcept;
	bool isValid() const noexcept;

	void setCount(uint8_t count) noexcept;

	std::string getHoverName() const;
	int16_t getDamageValue() const;

protected:
	template <typename T>
	T& field(std::size_t offset) noexcept {
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	template <typename T>
	const T& field(std::size_t offset) const noexcept {
		return *reinterpret_cast<const T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	std::byte mStorage[0x80];
};

static_assert(sizeof(ItemStackBase) == 0x80);
static_assert(alignof(ItemStackBase) == 8);

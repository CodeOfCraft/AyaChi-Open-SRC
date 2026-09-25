#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "BlockType.h"

class Block {
public:
	[[nodiscard]] BlockType* getBlockType() noexcept {
		return *reinterpret_cast<BlockType* const*>(
			reinterpret_cast<const std::byte*>(this) + 0x68);
	}

	[[nodiscard]] BlockType* getBlockType() const noexcept {
		return *reinterpret_cast<BlockType* const*>(
			reinterpret_cast<const std::byte*>(this) + 0x68);
	}

	[[nodiscard]] uint16_t getData() const noexcept {
		return field<const uint16_t>(0x120);
	}

	[[nodiscard]] const std::string& getTypeName() const noexcept {
		return getBlockType()->getTypeName();
	}

	[[nodiscard]] const std::string& getDescriptionId() const noexcept {
		return getBlockType()->getDescriptionId();
	}

	[[nodiscard]] uint64_t getSerializationIdHash() const noexcept {
		return field<const uint64_t>(0x108);
	}

	[[nodiscard]] uint32_t getSerializationIdHashForNetwork() const noexcept {
		return field<const uint32_t>(0x110);
	}

	[[nodiscard]] uint32_t getNetworkId() const noexcept {
		return field<const uint32_t>(0x114);
	}

	[[nodiscard]] bool hasProperty(BlockProperty property) const noexcept {
		const auto* type = getBlockType();
		return type != nullptr && type->hasProperty(property);
	}

	[[nodiscard]] bool isAir() const noexcept {
		const auto* type = getBlockType();
		if (type == nullptr)
			return true;
		const auto& name = type->getTypeName();
		return name == "minecraft:air" || name == "air";
	}

	bool operator==(const Block& other) const noexcept {
		return getSerializationIdHash() == other.getSerializationIdHash();
	}

	bool operator!=(const Block& other) const noexcept {
		return !(*this == other);
	}

private:
	template <typename T>
	const T& field(std::size_t offset) const noexcept {
		return *reinterpret_cast<const T*>(
			reinterpret_cast<uintptr_t>(this) + offset);
	}
};

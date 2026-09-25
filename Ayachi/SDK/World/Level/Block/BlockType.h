#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "../../../Core/Types/HashedString.h"
#include "../../../../Utils/Maths.h"
#include "../../../../Utils/MemoryUtil.h"
#include "../BlockSource.h"
#include "BlockProperty.h"

class BlockType {
public:
	[[nodiscard]] const std::string& getDescriptionId() const noexcept {
		return field<const std::string>(0x08);
	}

	[[nodiscard]] const HashedString& getFullName() const noexcept {
		return field<const HashedString>(0xE0);
	}

	[[nodiscard]] const std::string& getTypeName() const noexcept {
		return getFullName().getString();
	}

	[[nodiscard]] uint64_t getProperties() const noexcept {
		return field<const uint64_t>(0x130);
	}

	[[nodiscard]] bool hasProperty(BlockProperty property) const noexcept {
		return (getProperties() & static_cast<uint64_t>(property)) != 0;
	}

	[[nodiscard]] float getThickness() const noexcept {
		return field<const float>(0x158);
	}

	[[nodiscard]] float getTranslucency() const noexcept {
		return field<const float>(0x15C);
	}

	[[nodiscard]] uint8_t getRenderLayer() const noexcept {
		return field<const uint8_t>(0x162);
	}

	[[nodiscard]] uint8_t getLightBlock() const noexcept {
		return field<const uint8_t>(0x166);
	}

	[[nodiscard]] uint8_t getLightEmission() const noexcept {
		return field<const uint8_t>(0x167);
	}

	[[nodiscard]] float getFriction() const noexcept {
		return field<const float>(0x178);
	}

	[[nodiscard]] const Block* getNextBlockPermutation(const Block& block) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockType::getNextBlockPermutation,
			const Block*, const Block&>(const_cast<BlockType*>(this), block);
	}

	const AABB& getOutline(
		const Block& block,
		const BlockSource& blockSource,
		const BlockPos& position,
		AABB& result) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockType::getOutline,
			const AABB&,
			const Block&,
			const BlockSource&,
			const BlockPos&,
			AABB&>(const_cast<BlockType*>(this), block, blockSource, position, result);
	}

	const AABB& getVisualShapeInWorld(
		const Block& block,
		const BlockSource& blockSource,
		const BlockPos& position,
		AABB& result) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockType::getVisualShapeInWorld,
			const AABB&,
			const Block&,
			const BlockSource&,
			const BlockPos&,
			AABB&>(const_cast<BlockType*>(this), block, blockSource, position, result);
	}

	const AABB& getVisualShape(const Block& block, AABB& result) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockType::getVisualShape,
			const AABB&, const Block&, AABB&>(const_cast<BlockType*>(this), block, result);
	}

	const AABB& getUIShape(const Block& block, AABB& result) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockType::getUIShape,
			const AABB&, const Block&, AABB&>(const_cast<BlockType*>(this), block, result);
	}

private:
	template <typename T>
	const T& field(std::size_t offset) const noexcept {
		return *reinterpret_cast<const T*>(
			reinterpret_cast<uintptr_t>(this) + offset);
	}
};

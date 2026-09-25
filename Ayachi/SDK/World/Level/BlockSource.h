#pragma once

#include "../../../Utils/Maths.h"
#include "../../../Utils/MemoryUtil.h"
#include "../../Runtime/GameData.h"

class Block;

class BlockSource {
public:
	[[nodiscard]] const Block& getBlock(const BlockPos& position) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::getBlock,
			const Block&, const BlockPos&>(const_cast<BlockSource*>(this), position);
	}

	[[nodiscard]] const Block& getBlock(const BlockPos& position, uint32_t layer) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::getBlockLayer,
			const Block&, const BlockPos&, uint32_t>(
			const_cast<BlockSource*>(this), position, layer);
	}

	[[nodiscard]] const Block& getExtraBlock(const BlockPos& position) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::getExtraBlock,
			const Block&, const BlockPos&>(const_cast<BlockSource*>(this), position);
	}

	[[nodiscard]] const Block& getLiquidBlock(const BlockPos& position) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::getLiquidBlock,
			const Block&, const BlockPos&>(const_cast<BlockSource*>(this), position);
	}

	[[nodiscard]] bool hasBlock(const BlockPos& position) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::hasBlock,
			bool, const BlockPos&>(const_cast<BlockSource*>(this), position);
	}

	[[nodiscard]] bool containsAnyLiquid(const AABB& box) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::containsAnyLiquid,
			bool, const AABB&>(const_cast<BlockSource*>(this), box);
	}

	[[nodiscard]] bool isInWall(const Vec3<float>& position) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::isInWall,
			bool, const Vec3<float>&>(const_cast<BlockSource*>(this), position);
	}

	[[nodiscard]] bool isUnderWater(const BlockPos& position, const Block& block) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::isUnderWater,
			bool, const BlockPos&, const Block&>(
			const_cast<BlockSource*>(this), position, block);
	}

	[[nodiscard]] bool hasBorderBlock(const BlockPos& position) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::hasBorderBlock,
			bool, BlockPos>(const_cast<BlockSource*>(this), position);
	}

	[[nodiscard]] float getBrightness(const BlockPos& position) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::getBrightness,
			float, const BlockPos&>(const_cast<BlockSource*>(this), position);
	}

	[[nodiscard]] short getMinHeight() const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::getMinHeight,
			short>(const_cast<BlockSource*>(this));
	}

	[[nodiscard]] short getMaxHeight() const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::getMaxHeight,
			short>(const_cast<BlockSource*>(this));
	}

	[[nodiscard]] bool isSolidBlockingBlock(const BlockPos& position) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::isSolidBlockingBlock,
			bool, const BlockPos&>(const_cast<BlockSource*>(this), position);
	}

	[[nodiscard]] bool areChunksFullyLoaded(const BlockPos& position, int radius) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::BlockSource::areChunksFullyLoaded,
			bool, const BlockPos&, int>(const_cast<BlockSource*>(this), position, radius);
	}
};

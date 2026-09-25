#pragma once

#include <cstdint>

#include "../../../Utils/Maths.h"
#include "../../../Utils/MemoryUtil.h"
#include "../../Runtime/GameData.h"
#include "InteractionResult.h"

class Player;
class Actor;
class ItemStack;
class Block;

class GameMode {
public:
	Player* plr;
	BlockPos destroyBlockPos;
	uint8_t destroyBlockFace;
	float oldDestroyProgress;
	float destroyProgress;

	bool startDestroyBlock(const BlockPos& pos, uint8_t face, bool& canDestroy) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::startDestroyBlock,
			bool,
			const BlockPos&,
			uint8_t,
			bool&>(this, pos, face, canDestroy);
	}

	bool destroyBlock(const BlockPos& pos, uint8_t face) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::destroyBlock,
			bool,
			const BlockPos&,
			uint8_t>(this, pos, face);
	}

	bool continueDestroyBlock(const BlockPos& pos, uint8_t face, const Vec3<float>& playerPos, bool& canDestroy) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::continueDestroyBlock,
			bool,
			const BlockPos&,
			uint8_t,
			const Vec3<float>&,
			bool&>(this, pos, face, playerPos, canDestroy);
	}

	void stopDestroyBlock(const BlockPos& pos) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::stopDestroyBlock,
			void,
			const BlockPos&>(this, pos);
	}

	void startBuildBlock(const BlockPos& pos, uint8_t face) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::startBuildBlock,
			void,
			const BlockPos&,
			uint8_t>(this, pos, face);
	}

	bool buildBlock(const BlockPos& pos, uint8_t face, bool isSimTick) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::buildBlock,
			bool,
			const BlockPos&,
			uint8_t,
			bool>(this, pos, face, isSimTick);
	}

	void continueBuildBlock(const BlockPos& pos, uint8_t face) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::continueBuildBlock,
			void,
			const BlockPos&,
			uint8_t>(this, pos, face);
	}

	void stopBuildBlock() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::GameMode::stopBuildBlock, void>(this);
	}

	void tick() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::GameMode::tick, void>(this);
	}

	float getPickRange(const int& inputMode) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::getPickRange,
			float,
			const int&>(this, inputMode);
	}

	bool useItem(ItemStack& item) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::useItem,
			bool,
			ItemStack&>(this, item);
	}

	bool useItemAsAttack(ItemStack& item, const Vec3<float>& aimDirection) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::useItemAsAttack,
			bool,
			ItemStack&,
			const Vec3<float>&>(this, item, aimDirection);
	}

	InteractionResult useItemOn(
		ItemStack& item,
		const BlockPos& pos,
		uint8_t face,
		const Vec3<float>& hitPos,
		const Block* targetBlock,
		bool isFirstEvent) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::useItemOn,
			InteractionResult,
			ItemStack&,
			const BlockPos&,
			uint8_t,
			const Vec3<float>&,
			const Block*,
			bool>(this, item, pos, face, hitPos, targetBlock, isFirstEvent);
	}

	bool interact(Actor& actor, const Vec3<float>& location) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::interact,
			bool,
			Actor&,
			const Vec3<float>&>(this, actor, location);
	}

	bool attack(Actor& actor, const Vec3<float>& hitPosition) {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::GameMode::attack,
			bool,
			Actor&,
			const Vec3<float>&>(this, actor, hitPosition);
	}

	void releaseUsingItem() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::GameMode::releaseUsingItem, void>(this);
	}
};

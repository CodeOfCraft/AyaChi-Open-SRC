#pragma once

#include <cstdint>

#include "../Runtime/GameData.h"

class BaseActorRenderContext;
class Matrix;
class Player;

enum class ItemContextFlags : std::uint8_t {
	None = 0,
	FirstPersonPass = 1 << 0,
	WorldPass = 1 << 1,
	UIPass = 1 << 2,
	Item2D = 1 << 3,
	Item3D = 1 << 4,
	InHand = 1 << 5,
	Glint = 1 << 6,
	MultiColorTint = 1 << 7,
};

class ItemInHandRenderer {
public:
	void renderFirstPerson(
		BaseActorRenderContext& renderContext,
		const Matrix& previousProjection,
		ItemContextFlags itemFlags) {
		if (GameData::Address::ItemInHandRenderer_renderFirstPerson == 0)
			return;

		using Function = void(__fastcall*)(
			ItemInHandRenderer*, BaseActorRenderContext&, const Matrix&, ItemContextFlags);
		reinterpret_cast<Function>(GameData::Address::ItemInHandRenderer_renderFirstPerson)(
			this, renderContext, previousProjection, itemFlags);
	}

	void renderOffhandItem(
		BaseActorRenderContext& renderContext,
		Player& player,
		ItemContextFlags itemFlags) {
		if (GameData::Address::ItemInHandRenderer_renderOffhandItem == 0)
			return;

		using Function = void(__fastcall*)(
			ItemInHandRenderer*, BaseActorRenderContext&, Player&, ItemContextFlags);
		reinterpret_cast<Function>(GameData::Address::ItemInHandRenderer_renderOffhandItem)(
			this, renderContext, player, itemFlags);
	}
};

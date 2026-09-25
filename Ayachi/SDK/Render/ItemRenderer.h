#pragma once

#include "../Runtime/GameData.h"
#include "../../Utils/MemoryUtil.h"

class BaseActorRenderContext;
struct ActorRenderData;
class ItemStack;

class ItemRenderer {
public:
	void render(BaseActorRenderContext& renderContext, ActorRenderData& renderData) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::ItemRenderer::render,
			void,
			BaseActorRenderContext&,
			ActorRenderData&>(this, renderContext, renderData);
	}

	void renderGuiItemNew(
		BaseActorRenderContext& renderContext,
		const ItemStack& itemStack,
		int frame,
		float x,
		float y,
		bool renderEnchantmentFoil,
		float transparency,
		float lightMultiplier,
		float scale,
		int zOrder = 17) {
		if (GameData::Address::ItemRenderer_renderGuiItemNew == 0)
			return;

		using Function = void(__fastcall*)(
			ItemRenderer*, BaseActorRenderContext&, const ItemStack&, int,
			float, float, bool, float, float, float, int);
		reinterpret_cast<Function>(GameData::Address::ItemRenderer_renderGuiItemNew)(
			this,
			renderContext,
			itemStack,
			frame,
			x,
			y,
			renderEnchantmentFoil,
			transparency,
			lightMultiplier,
			scale,
			zOrder);
	}
};

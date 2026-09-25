#pragma once

#include "../../Utils/MemoryUtil.h"
#include "../Core/Types/mce.h"
#include "RectangleArea.h"

class ClientInstance;
class ScreenContext;

class MinecraftUIRenderContext {
public:
	[[nodiscard]] ClientInstance* getClientInstance() const {
		return *reinterpret_cast<ClientInstance* const*>(reinterpret_cast<uintptr_t>(this) + 0x8);
	}

	[[nodiscard]] ScreenContext* getScreenContext() const {
		return *reinterpret_cast<ScreenContext* const*>(reinterpret_cast<uintptr_t>(this) + 0x10);
	}

	[[nodiscard]] float getTextAlpha() const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::getTextAlpha,
			float>(const_cast<MinecraftUIRenderContext*>(this));
	}

	void setTextAlpha(float alpha) {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::setTextAlpha, void, float>(
			this, alpha);
	}

	void increaseStencilRef() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::increaseStencilRef, void>(this);
	}

	void decreaseStencilRef() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::decreaseStencilRef, void>(this);
	}

	void resetStencilRef() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::resetStencilRef, void>(this);
	}

	void drawRectangle(const RectangleArea& area, const mce::Color& color, float alpha, int thickness) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::drawRectangle,
			void,
			const RectangleArea&,
			const mce::Color&,
			float,
			int>(this, area, color, alpha, thickness);
	}

	void fillRectangle(const RectangleArea& area, const mce::Color& color, float alpha) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::fillRectangle,
			void,
			const RectangleArea&,
			const mce::Color&,
			float>(this, area, color, alpha);
	}

	void fillRectangleStencil(const RectangleArea& area) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::fillRectangleStencil,
			void,
			const RectangleArea&>(this, area);
	}

	void enableScissorTest(const RectangleArea& area) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::enableScissorTest,
			void,
			const RectangleArea&>(this, area);
	}

	void disableScissorTest() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::disableScissorTest, void>(this);
	}

	void setClippingRectangle(const RectangleArea& area) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::setClippingRectangle,
			void,
			const RectangleArea&>(this, area);
	}

	void setFullClippingRectangle() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::setFullClippingRectangle, void>(this);
	}

	void saveCurrentClippingRectangle() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::saveCurrentClippingRectangle, void>(this);
	}

	void restoreSavedClippingRectangle() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::restoreSavedClippingRectangle, void>(this);
	}

	void cleanup() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::cleanup, void>(this);
	}

	void removePersistentMeshes() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::removePersistentMeshes, void>(this);
	}
};

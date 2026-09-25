#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "../../Libs/glm/glm/glm.hpp"
#include "../../Utils/MemoryUtil.h"
#include "../Core/Types/mce.h"
#include "CaretMeasureData.h"
#include "RectangleArea.h"
#include "TextMeasureData.h"

class ClientInstance;
class Font;
class HashedString;
class ScreenContext;
struct NinesliceInfo;
namespace mce { struct ClientTexture; }

class MinecraftUIRenderContext {
public:
	[[nodiscard]] ClientInstance* getClientInstance() const {
		return *reinterpret_cast<ClientInstance* const*>(reinterpret_cast<uintptr_t>(this) + 0x8);
	}

	[[nodiscard]] ScreenContext* getScreenContext() const {
		return *reinterpret_cast<ScreenContext* const*>(reinterpret_cast<uintptr_t>(this) + 0x10);
	}

	[[nodiscard]] int getLineLength(
		Font& font, const std::string& text, float fontSize, bool showColorSymbol = false) const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::getLineLength,
			int, Font&, const std::string&, float, bool>(
			const_cast<MinecraftUIRenderContext*>(this), font, text, fontSize, showColorSymbol);
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

	void drawDebugText(
		const RectangleArea& area, std::string text, const mce::Color& color, float alpha,
		ui::TextAlignment alignment, const TextMeasureData& textData,
		const CaretMeasureData& caretData) {
		using Function = void(__fastcall*)(
			MinecraftUIRenderContext*, const RectangleArea&, std::string&&, const mce::Color&,
			float, ui::TextAlignment, const TextMeasureData&, const CaretMeasureData&);
		void** vtable = *reinterpret_cast<void***>(this);
		reinterpret_cast<Function>(vtable[GameData::VTableIndex::UIRenderContext::drawDebugText])(
			this, area, std::move(text), color, alpha, alignment, textData, caretData);
	}

	void drawText(
		Font& font, const RectangleArea& area, std::string text, const mce::Color& color,
		float alpha, ui::TextAlignment alignment, const TextMeasureData& textData,
		const CaretMeasureData& caretData) {
		using Function = void(__fastcall*)(
			MinecraftUIRenderContext*, Font&, const RectangleArea&, std::string&&,
			const mce::Color&, float, ui::TextAlignment, const TextMeasureData&,
			const CaretMeasureData&);
		void** vtable = *reinterpret_cast<void***>(this);
		reinterpret_cast<Function>(vtable[GameData::VTableIndex::UIRenderContext::drawText])(
			this, font, area, std::move(text), color, alpha, alignment, textData, caretData);
	}

	void flushText(float deltaTime, std::optional<float> obfuscateSwitchTime = std::nullopt) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::flushText,
			void, float, std::optional<float>>(this, deltaTime, obfuscateSwitchTime);
	}

	void drawImage(
		const mce::ClientTexture& texture, const glm::vec2& position, const glm::vec2& size,
		const glm::vec2& uv, const glm::vec2& uvSize, bool colorCorrected = false) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::drawImage,
			void, const mce::ClientTexture&, const glm::vec2&, const glm::vec2&,
			const glm::vec2&, const glm::vec2&, bool>(
			this, texture, position, size, uv, uvSize, colorCorrected);
	}

	void drawNineslice(const mce::ClientTexture& texture, const NinesliceInfo& info) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::drawNineslice,
			void, const mce::ClientTexture&, const NinesliceInfo&>(this, texture, info);
	}

	void flushImages(const mce::Color& color, float alpha, const HashedString& materialNameHash) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::flushImages,
			void, const mce::Color&, float, const HashedString&>(
			this, color, alpha, materialNameHash);
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

	[[nodiscard]] RectangleArea getFullClippingRectangle() const {
		return MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::getFullClippingRectangle,
			RectangleArea>(const_cast<MinecraftUIRenderContext*>(this));
	}

	void snapImageSizeToGrid(glm::vec2& size) const {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::snapImageSizeToGrid,
			void, glm::vec2&>(const_cast<MinecraftUIRenderContext*>(this), size);
	}

	void snapImagePositionToGrid(glm::vec2& position) const {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::snapImagePositionToGrid,
			void, glm::vec2&>(const_cast<MinecraftUIRenderContext*>(this), position);
	}

	void notifyImageEstimate(std::uint64_t imageCount) {
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::UIRenderContext::notifyImageEstimate,
			void, std::uint64_t>(this, imageCount);
	}

	void cleanup() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::cleanup, void>(this);
	}

	void removePersistentMeshes() {
		MemoryUtil::CallVFunc<GameData::VTableIndex::UIRenderContext::removePersistentMeshes, void>(this);
	}
};

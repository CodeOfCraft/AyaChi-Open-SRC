#pragma once

#include <utility>

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Render/MinecraftUIRenderContext.h"
#include "../../../../../SDK/Render/TextMeasureData.h"
#include "../../../../../SDK/Render/CaretMeasureData.h"
#include "../../../../../Utils/HookGuard.h"

class Font;

class DrawTextHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		MinecraftUIRenderContext*,
		Font&,
		const RectangleArea&,
		std::string&&,
		const mce::Color&,
		float,
		ui::TextAlignment,
		const TextMeasureData&,
		const CaretMeasureData&);
	static inline func_t oFunc = nullptr;

	static void drawTextCallback(
		MinecraftUIRenderContext* context,
		Font& font,
		const RectangleArea& area,
		std::string&& text,
		const mce::Color& color,
		float alpha,
		ui::TextAlignment alignment,
		const TextMeasureData& textData,
		const CaretMeasureData& caretData) {
		HookGuard::Scope guard;
		if (!guard) {
			oFunc(context, font, area, std::move(text), color, alpha, alignment, textData, caretData);
			return;
		}

		bool cancel = false;
		ModuleManager::onDrawText(
			context, font, area, text, color, alpha, alignment, textData, caretData, cancel);
		if (!cancel)
			oFunc(context, font, area, std::move(text), color, alpha, alignment, textData, caretData);
	}

public:
	DrawTextHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&drawTextCallback);
	}
};

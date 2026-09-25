#pragma once

namespace ui {
	enum class TextAlignment : int {
		Left = 0,
		Right = 1,
		Center = 2,
	};
}

struct TextMeasureData {
	float fontSize;
	float linePadding;
	bool renderShadow;
	bool showColorSymbol;
	bool hideHyphen;
	ui::TextAlignment alignment;
};

static_assert(sizeof(TextMeasureData) == 0x10);

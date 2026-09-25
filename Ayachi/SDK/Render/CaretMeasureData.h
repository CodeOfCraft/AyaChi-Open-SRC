#pragma once

struct CaretMeasureData {
	int position;
	bool shouldRender;
};

static_assert(sizeof(CaretMeasureData) == 0x8);

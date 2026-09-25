#pragma once

struct RectangleArea {
	float left;
	float right;
	float top;
	float bottom;

	constexpr RectangleArea(float leftValue = 0.f, float topValue = 0.f,
		float rightValue = 0.f, float bottomValue = 0.f)
		: left(leftValue), right(rightValue), top(topValue), bottom(bottomValue) {
	}
};

static_assert(sizeof(RectangleArea) == 0x10);

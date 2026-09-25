#pragma once
#include <string>
#include <stdint.h>

#include "../SDK/Core/Types/mce.h"
#include <imgui.h>

struct UIColor {
	union
	{
		struct {
			uint8_t r, g, b, a;
		};
		int arr[4];
	};
	UIColor(uint8_t red = 255, uint8_t green = 255, uint8_t blue = 255, uint8_t alpha = 255) {
		this->r = red;
		this->g = green;
		this->b = blue;
		this->a = alpha;
	}

	UIColor(const mce::Color& color) {
		this->r = (int)(color.r * 255.f);
		this->g = (int)(color.g * 255.f);
		this->b = (int)(color.b * 255.f);
		this->a = (int)(color.a * 255.f);
	}

	bool operator==(const UIColor& other) const {
		return (r == other.r && g == other.g && b == other.b && a == other.a);
	}

	mce::Color toMCColor() const {
		return mce::Color((float)r / 255.f, (float)g / 255.f, (float)b / 255.f, (float)a / 255.f);
	}

	ImColor toImColor(int alpha = -1) const {
		int finalAlpha = (alpha < 0) ? this->a : std::clamp(alpha, 0, 255);
		return ImColor(this->r, this->g, this->b, static_cast<uint8_t>(finalAlpha));
	}
};

namespace ColorUtil {
	uint32_t ColorToUInt(const UIColor& color);
	std::string ColorToHexString(const UIColor& color);
	UIColor HexStringToColor(std::string hexString);
	void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);
	UIColor lerp(const UIColor& start, const UIColor& end, float t);
	UIColor getRainbowColor(float seconds, float saturation, float brightness, long index);
	UIColor getWaveColor(const UIColor& startColor, const UIColor& endColor, long index);
	inline UIColor toUIColor(const mce::Color& color) {
		UIColor result;
		result.r = (int)(color.r * 255.f);
		result.g = (int)(color.g * 255.f);
		result.b = (int)(color.b * 255.f);
		result.a = (int)(color.a * 255.f);
		return result;
	}
}

#include "ColorUtil.h"
#include "TimerUtil.h"
#include <charconv>
#include <math.h>

uint32_t ColorUtil::ColorToUInt(const UIColor& color) {
	return (static_cast<uint32_t>(color.a) << 24) |
		(static_cast<uint32_t>(color.r) << 16) |
		(static_cast<uint32_t>(color.g) << 8) |
		color.b;
}

std::string ColorUtil::ColorToHexString(const UIColor& color) {
	std::stringstream ss;
	ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.r);
	ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.g);
	ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.b);
	ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.a);

	return ss.str();
}

UIColor ColorUtil::HexStringToColor(std::string hexString) {
	if (hexString.starts_with("0x") || hexString.starts_with("0X"))
		hexString.erase(0, 2);

	if (hexString.length() != 8)
		return UIColor(0, 0, 0);

	uint32_t colorValue = 0;
	const auto [end, error] = std::from_chars(hexString.data(), hexString.data() + hexString.size(),
		colorValue, 16);
	if (error != std::errc{} || end != hexString.data() + hexString.size())
		return UIColor(0, 0, 0);

	return UIColor(
		static_cast<uint8_t>((colorValue >> 24) & 0xFF),
		static_cast<uint8_t>((colorValue >> 16) & 0xFF),
		static_cast<uint8_t>((colorValue >> 8) & 0xFF),
		static_cast<uint8_t>(colorValue & 0xFF));
}

void ColorUtil::ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b) {
	if (s == 0.0f) {
		out_r = out_g = out_b = v;
		return;
	}

	h = fmodf(h, 1.0f);
	if (h < 0.f)
		h += 1.f;
	h *= 6.f;
	const int i = static_cast<int>(h);
	const float f = h - static_cast<float>(i);
	const float p = v * (1.0f - s);
	const float q = v * (1.0f - s * f);
	const float t = v * (1.0f - s * (1.0f - f));

	switch (i) {
	case 0:
		out_r = v;
		out_g = t;
		out_b = p;
		break;
	case 1:
		out_r = q;
		out_g = v;
		out_b = p;
		break;
	case 2:
		out_r = p;
		out_g = v;
		out_b = t;
		break;
	case 3:
		out_r = p;
		out_g = q;
		out_b = v;
		break;
	case 4:
		out_r = t;
		out_g = p;
		out_b = v;
		break;
	case 5:
	default:
		out_r = v;
		out_g = p;
		out_b = q;
		break;
	}
}

UIColor ColorUtil::lerp(const UIColor& start, const UIColor& end, float t) {
	UIColor result;
	result.r = start.r + (int)((end.r - start.r) * t);
	result.g = start.g + (int)((end.g - start.g) * t);
	result.b = start.b + (int)((end.b - start.b) * t);
	result.a = start.a + (int)((end.a - start.a) * t);
	return result;
}

UIColor ColorUtil::getRainbowColor(float seconds, float saturation, float brightness, long index) {
	seconds = std::max(seconds, 0.001f);
	const double elapsedSeconds = (static_cast<double>(TimerUtil::getCurrentMs())
		+ static_cast<double>(index)) / 1000.0;
	const float currentHue = static_cast<float>(fmod(elapsedSeconds, seconds) / seconds);
	float red = 0.f;
	float green = 0.f;
	float blue = 0.f;
	ColorConvertHSVtoRGB(currentHue, saturation, brightness, red, green, blue);

	return UIColor(static_cast<int>(red * 255.f), static_cast<int>(green * 255.f), static_cast<int>(blue * 255.f));
}

UIColor ColorUtil::getWaveColor(const UIColor& startColor, const UIColor& endColor, long index) {
	const double elapsedMs = static_cast<double>(TimerUtil::getCurrentMs()) - static_cast<double>(index);
	double phase = fmod(elapsedMs / 1000.0, 2.0);
	if (phase < 0.0)
		phase += 2.0;
	const double amount = phase <= 1.0 ? phase : 2.0 - phase;
	const double inverse = 1.0 - amount;
	const int redPart = static_cast<int>(startColor.r * inverse + endColor.r * amount);
	const int greenPart = static_cast<int>(startColor.g * inverse + endColor.g * amount);
	const int bluePart = static_cast<int>(startColor.b * inverse + endColor.b * amount);
	const int alphaPart = static_cast<int>(startColor.a * inverse + endColor.a * amount);
	return UIColor(redPart, greenPart, bluePart, alphaPart);
}

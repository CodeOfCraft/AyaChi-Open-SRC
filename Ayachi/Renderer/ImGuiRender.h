#pragma once
#include <string>
#include "../Libs/ImGui/imgui.h"
#include "../Utils/Maths.h"
#include "../Utils/ColorUtil.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Texture2D;

class ImGuiRender {
private:
	static inline ImDrawList* drawlist = nullptr;
	static inline float fontSize = 25.f;
	static inline float shadowOffset = 1.f;
public:
	static void beginFrame(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer);
	static void endFrame();
	static void releaseBlur();

	static void setExternalBlurTexture(ImTextureID id);
	static void setDrawList(ImDrawList* d);
	static void drawText(Vec2<float> textPos, const std::string& textStr, UIColor color, float textSize, bool glow = false, float thick_ness = 0.f);
	static void setFontSize(float newSize) { fontSize = newSize; };
	static void setShadowOffset(float offset) { shadowOffset = offset; }
	static float getTextWidth(const std::string& textStr, float textSize = 1.f);
	static float getTextHeight(float textSize = 1.f);
	static void drawLine(Vec2<float> start, Vec2<float> end, UIColor color, float width = 1.f);
	static void drawRectangle(const Vec4<float>& rectPos, UIColor color, float lineWidth = 1.0f, float rounding = 0.0f, ImDrawFlags flags = 0);
	static void drawRectangle(Vec2<float> pMin, Vec2<float> pMax, UIColor color, float lineWidth = 1.0f, float rounding = 0.0f, ImDrawFlags flags = 0);
	static void fillRectangle(Vec4<float> rectPos, UIColor color, float rounding = 0.0f, ImDrawFlags flags = 0);
	static void fillRectangle(Vec2<float> pMin, Vec2<float> pMax, UIColor color, float rounding = 0.0f, ImDrawFlags flags = 0);
	static void drawBlur(const Vec4<float>& rectPos, float rounding = 0.0f, float alpha = 1.0f, ImDrawFlags flags = 0);
	static void drawBlur(Vec2<float> pMin, Vec2<float> pMax, float rounding = 0.0f, float alpha = 1.0f, ImDrawFlags flags = 0);
	static bool worldToScreen(const Vec3<float>& pos, Vec2<float>& out);
	static bool drawBox3D(
		ImDrawList* drawList,
		const AABB& box,
		UIColor fillColor,
		UIColor lineColor,
		float lineWidth = 1.f,
		float scale = 1.f);
	static void renderCircle(Vec2<float> pos, ImColor color, float radius = 1.f, float thickness = 1.f, int num_seg = 0);
	static void renderCircleFilled(Vec2<float> pos, ImColor color, float radius = 1.f, int num_seg = 0);
};

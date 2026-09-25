#pragma once

#include <optional>

#include "../SDK/Core/GUI/ScreenView.h"
#include "../SDK/Render/BaseActorRenderContext.h"
#include "../SDK/Render/ItemRenderer.h"
#include "../SDK/Render/MinecraftUIRenderContext.h"
#include "../Utils/ColorUtil.h"
#include "../Utils/Maths.h"

class ItemStack;

class MCRender {
public:
	[[nodiscard]] static bool beginFrame(ScreenView* view, MinecraftUIRenderContext* context);
	static void endFrame();

	[[nodiscard]] static bool isFrameReady();
	[[nodiscard]] static ScreenView* getScreenView();
	[[nodiscard]] static MinecraftUIRenderContext* getRenderContext();
	[[nodiscard]] static BaseActorRenderContext* getBaseActorRenderContext();
	[[nodiscard]] static ItemRenderer* getItemRenderer();
	[[nodiscard]] static float getDeltaTime();

	static void drawRectangle(const RectangleArea& area, const UIColor& color, int thickness = 1);
	static void drawRectangle(const Vec4<float>& area, const UIColor& color, int thickness = 1);
	static void fillRectangle(const RectangleArea& area, const UIColor& color);
	static void fillRectangle(const Vec4<float>& area, const UIColor& color);

	static void fillRectangleStencil(const RectangleArea& area);
	static void enableScissorTest(const RectangleArea& area);
	static void disableScissorTest();
	static void setClippingRectangle(const RectangleArea& area);
	static void setFullClippingRectangle();
	static void drawItem(
		const Vec2<float>& position,
		const ItemStack* itemStack,
		float scale = 1.f,
		float opacity = 1.f,
		bool renderEnchantmentFoil = false,
		int frame = 0,
		int zOrder = 17);

private:
	[[nodiscard]] static bool isRenderLayer(const ScreenView* view);
	[[nodiscard]] static RectangleArea toRectangleArea(const Vec4<float>& area);

	static inline ScreenView* screenView = nullptr;
	static inline MinecraftUIRenderContext* renderContext = nullptr;
	static inline std::optional<BaseActorRenderContext> baseActorRenderContext;
	static inline ItemRenderer* itemRenderer = nullptr;
	static inline float deltaTime = 0.f;
};

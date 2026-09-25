#include "../pch.h"
#include "MCRender.h"
#include "../SDK/Core/Client/ClientInstance.h"
#include "../SDK/World/Item/ItemStack.h"

bool MCRender::beginFrame(ScreenView* view, MinecraftUIRenderContext* context) {
	endFrame();
	if (view == nullptr || context == nullptr || !isRenderLayer(view))
		return false;

	screenView = view;
	renderContext = context;
	deltaTime = view->getDeltaTime();

	ClientInstance* clientInstance = context->getClientInstance();
	ScreenContext* screenContext = context->getScreenContext();
	if (clientInstance != nullptr) {
		itemRenderer = clientInstance->getItemRenderer();
		MinecraftGame* minecraftGame = clientInstance->getMinecraftGame();
		if (screenContext != nullptr && minecraftGame != nullptr &&
			GameData::Address::BaseActorRenderContext_Constructor != 0) {
			baseActorRenderContext.emplace(screenContext, clientInstance, minecraftGame);
			if (!baseActorRenderContext->isValid()) {
				baseActorRenderContext.reset();
			} else if (ItemRenderer* contextItemRenderer = baseActorRenderContext->getItemRenderer()) {
				itemRenderer = contextItemRenderer;
			}
		}
	}
	return true;
}

void MCRender::endFrame() {
	baseActorRenderContext.reset();
	itemRenderer = nullptr;
	screenView = nullptr;
	renderContext = nullptr;
	deltaTime = 0.f;
}

bool MCRender::isFrameReady() {
	return screenView != nullptr && renderContext != nullptr;
}

ScreenView* MCRender::getScreenView() {
	return screenView;
}

MinecraftUIRenderContext* MCRender::getRenderContext() {
	return renderContext;
}

BaseActorRenderContext* MCRender::getBaseActorRenderContext() {
	return baseActorRenderContext ? &*baseActorRenderContext : nullptr;
}

ItemRenderer* MCRender::getItemRenderer() {
	return itemRenderer;
}

float MCRender::getDeltaTime() {
	return deltaTime;
}

void MCRender::drawRectangle(const RectangleArea& area, const UIColor& color, int thickness) {
	if (renderContext == nullptr)
		return;

	const mce::Color mcColor = color.toMCColor();
	renderContext->drawRectangle(area, mcColor, mcColor.a, thickness);
}

void MCRender::drawRectangle(const Vec4<float>& area, const UIColor& color, int thickness) {
	drawRectangle(toRectangleArea(area), color, thickness);
}

void MCRender::fillRectangle(const RectangleArea& area, const UIColor& color) {
	if (renderContext == nullptr)
		return;

	const mce::Color mcColor = color.toMCColor();
	renderContext->fillRectangle(area, mcColor, mcColor.a);
}

void MCRender::fillRectangle(const Vec4<float>& area, const UIColor& color) {
	fillRectangle(toRectangleArea(area), color);
}

void MCRender::fillRectangleStencil(const RectangleArea& area) {
	if (renderContext != nullptr)
		renderContext->fillRectangleStencil(area);
}

void MCRender::enableScissorTest(const RectangleArea& area) {
	if (renderContext != nullptr)
		renderContext->enableScissorTest(area);
}

void MCRender::disableScissorTest() {
	if (renderContext != nullptr)
		renderContext->disableScissorTest();
}

void MCRender::setClippingRectangle(const RectangleArea& area) {
	if (renderContext != nullptr)
		renderContext->setClippingRectangle(area);
}

void MCRender::setFullClippingRectangle() {
	if (renderContext != nullptr)
		renderContext->setFullClippingRectangle();
}

void MCRender::drawItem(
	const Vec2<float>& position,
	const ItemStack* itemStack,
	float scale,
	float opacity,
	bool renderEnchantmentFoil,
	int frame,
	int zOrder) {
	BaseActorRenderContext* baseContext = getBaseActorRenderContext();
	if (itemRenderer == nullptr || baseContext == nullptr || itemStack == nullptr)
		return;

	itemRenderer->renderGuiItemNew(
		*baseContext,
		*itemStack,
		frame,
		position.x,
		position.y,
		renderEnchantmentFoil,
		opacity,
		1.f,
		scale,
		zOrder);
}

bool MCRender::isRenderLayer(const ScreenView* view) {
	if (view->isScreen("debug_screen"))
		return true;

	const VisualTree* tree = view->getVisualTree();
	return tree != nullptr && tree->getRootControlName() == "debug_screen.debug_screen";
}

RectangleArea MCRender::toRectangleArea(const Vec4<float>& area) {
	return RectangleArea(area.x, area.y, area.z, area.w);
}

#include "Arraylist.h"

#include <algorithm>
#include <cmath>

#include "../Client/ClickGUI.h"
#include "../Client/Colors.h"
#include "../Client/HudEditor.h"
#include "../../../ModuleManager.h"
#include "../../../../../../Renderer/ImGuiRender.h"

namespace {
	constexpr const char* HudElementId = "Arraylist";

	struct ArraylistEntry {
		std::string name;
		std::string modeText;
		float nameWidth = 0.f;
		float totalWidth = 0.f;
		float alpha = 0.f;
		UIColor color;
	};

	int alphaByte(float alpha, float value = 255.f) {
		return std::clamp(static_cast<int>(alpha * value), 0, 255);
	}

	Vec4<float> snapRect(const Vec4<float>& rect) {
		return Vec4<float>(
			std::floor(rect.x),
			std::floor(rect.y),
			std::ceil(rect.z),
			std::ceil(rect.w));
	}

	void drawGlowRect(
		ImDrawList* drawlist,
		const Vec4<float>& rect,
		const UIColor& color,
		float alpha,
		float rounding,
		ImDrawFlags flags,
		float strength) {
		if (drawlist == nullptr || alpha <= 0.f || strength <= 0.f)
			return;

		const float spread = 7.f * strength;
		for (int index = 4; index >= 1; --index) {
			const float progress = static_cast<float>(index) / 4.f;
			const float expand = spread * progress;
			const int glowAlpha = alphaByte(
				alpha, 28.f * strength * (1.f - progress * 0.55f));
			drawlist->AddRectFilled(
				ImVec2(rect.x - expand, rect.y - expand),
				ImVec2(rect.z + expand, rect.w + expand),
				color.toImColor(glowAlpha),
				rounding + expand,
				flags);
		}
	}

	void drawGlowText(
		const Vec2<float>& position,
		const std::string& text,
		const UIColor& color,
		float textSize,
		float alpha,
		float strength) {
		if (text.empty() || alpha <= 0.f || strength <= 0.f)
			return;

		const UIColor glowColor(
			color.r, color.g, color.b,
			static_cast<uint8_t>(alphaByte(alpha, 66.f * strength)));
		const float glowOffset = std::max(0.75f, 1.35f * strength);
		ImGuiRender::drawText(
			Vec2<float>(position.x - glowOffset, position.y - glowOffset), text, glowColor, textSize);
		ImGuiRender::drawText(
			Vec2<float>(position.x + glowOffset, position.y - glowOffset), text, glowColor, textSize);
		ImGuiRender::drawText(
			Vec2<float>(position.x - glowOffset, position.y + glowOffset), text, glowColor, textSize);
		ImGuiRender::drawText(
			Vec2<float>(position.x + glowOffset, position.y + glowOffset), text, glowColor, textSize);
	}

	ImDrawFlags getRowFlags(bool rounded, bool bottom, bool first, bool last) {
		if (!rounded || (!first && !last))
			return 0;
		if (first && last)
			return ImDrawFlags_RoundCornersAll;
		if (bottom)
			return first ? ImDrawFlags_RoundCornersBottom : ImDrawFlags_RoundCornersTop;
		return first ? ImDrawFlags_RoundCornersTop : ImDrawFlags_RoundCornersBottom;
	}

	ImDrawFlags getBarFlags(bool rounded, bool bottom, bool first, bool last) {
		if (!rounded || (!first && !last))
			return 0;
		if (first && last)
			return ImDrawFlags_RoundCornersRight;
		if (bottom)
			return first ? ImDrawFlags_RoundCornersBottomRight : ImDrawFlags_RoundCornersTopRight;
		return first ? ImDrawFlags_RoundCornersTopRight : ImDrawFlags_RoundCornersBottomRight;
	}
}

Arraylist::Arraylist()
	: Module("Arraylist", "Display enabled modules", Category::RENDER) {
	registerSetting(new ColorSetting(
		"TextColor", "Module name text color", &textColor, UIColor(255, 255, 255), true, true));
	registerSetting(new SliderSetting<float>(
		"Opacity", "Background opacity", &bgOpacity, 0.58f, 0.f, 1.f));
	registerSetting(new BoolSetting("Rounded", "Round outer corners", &rounded, true));
	registerSetting(new SliderSetting<float>(
		"Rounding", "Corner radius", &rounding, 4.f, 0.f, 15.f,
		[this]() { return rounded; }));
	registerSetting(new SliderSetting<float>("Scale", "List scale", &scale, 1.f, 0.1f, 2.f));
	registerSetting(new BoolSetting("Glow", "Text and accent glow", &glow, true));
	registerSetting(new SliderSetting<float>(
		"GlowStrength", "Glow strength", &glowStrength, 0.8f, 0.1f, 2.f,
		[this]() { return glow; }));
	registerSetting(new BoolSetting("Bottom", "Align the list to the bottom", &bottom, false));
	HudEditor::setVisible(HudElementId, false);
}

void Arraylist::onEnable() {
	HudEditor::setVisible(HudElementId, true);
}

void Arraylist::onDisable() {
	HudEditor::setVisible(HudElementId, false);
	HudEditor::setActualSize(HudElementId, Vec2<float>());
}

void Arraylist::onImGuiRender(ImDrawList* drawlist) {
	static ClickGUI* clickGui = ModuleManager::getModule<ClickGUI>();
	static Colors* colors = ModuleManager::getModule<Colors>();
	if (drawlist == nullptr || colors == nullptr || (clickGui != nullptr && clickGui->isEnabled()))
		return;

	const float safeScale = std::max(scale, 0.1f);
	const float textSize = safeScale;
	const float paddingX = 5.f * safeScale;
	const float paddingY = 2.f * safeScale;
	const float barWidth = 2.5f * safeScale;
	const float textHeight = ImGuiRender::getTextHeight(textSize);
	const float moduleHeight = textHeight + paddingY * 2.f;
	const float rowRounding = rounded ? rounding * safeScale : 0.f;
	const float glowPower = std::clamp(glowStrength, 0.1f, 2.f);

	ImGuiIO& io = ImGui::GetIO();
	GuiData* guiData = Game::getGuiData();
	const float screenWidth = guiData != nullptr && guiData->screenSize.x > 0.f
		? guiData->screenSize.x
		: io.DisplaySize.x;
	const float screenHeight = guiData != nullptr && guiData->screenSize.y > 0.f
		? guiData->screenSize.y
		: io.DisplaySize.y;
	if (screenWidth <= 0.f || screenHeight <= 0.f)
		return;

	const float frameTime = std::clamp(io.DeltaTime, 0.f, 0.1f);
	const float animationStep = 1.f - std::exp(-15.f * frameTime);
	thread_local std::vector<ArraylistEntry> renderList;
	if (renderList.capacity() < ModuleManager::moduleList.size())
		renderList.reserve(ModuleManager::moduleList.size());

	std::size_t renderCount = 0;
	for (Module* module : ModuleManager::moduleList) {
		if (module == nullptr)
			continue;

		const bool shouldRender = module->isEnabled() && module->isVisible();
		const float targetAlpha = shouldRender ? 1.f : 0.f;
		module->arraylistAnim = Math::lerp(module->arraylistAnim, targetAlpha, animationStep);
		if (std::abs(module->arraylistAnim - targetAlpha) < 0.001f)
			module->arraylistAnim = targetAlpha;
		if (module->arraylistAnim < 0.001f)
			continue;

		if (renderCount == renderList.size())
			renderList.emplace_back();
		ArraylistEntry& entry = renderList[renderCount++];
		entry.name = module->getDisplayName();
		entry.modeText.clear();
		const std::string mode = module->getModeText();
		if (mode != "NULL")
			entry.modeText = " " + mode;
		entry.nameWidth = ImGuiRender::getTextWidth(entry.name, textSize);
		const float modeWidth = entry.modeText.empty()
			? 0.f
			: ImGuiRender::getTextWidth(entry.modeText, textSize);
		entry.totalWidth = entry.nameWidth + modeWidth + paddingX * 2.f + barWidth;
		entry.alpha = std::clamp(module->arraylistAnim, 0.f, 1.f);
	}

	std::sort(renderList.begin(), renderList.begin() + renderCount, [](const auto& lhs, const auto& rhs) {
		if (std::abs(lhs.totalWidth - rhs.totalWidth) > 0.5f)
			return lhs.totalWidth > rhs.totalWidth;
		return lhs.name < rhs.name;
	});

	float listWidth = 0.f;
	float listHeight = 0.f;
	for (std::size_t index = 0; index < renderCount; ++index) {
		ArraylistEntry& entry = renderList[index];
		entry.color = colors->getColor(-static_cast<int>(index) * colors->getSeperation());
		listWidth = std::max(listWidth, entry.totalWidth);
		listHeight += moduleHeight * entry.alpha;
	}

	HudEditor::setVisible(HudElementId, true);
	HudEditor::setActualSize(HudElementId, Vec2<float>(listWidth, listHeight));
	HudEditor::setDefaultPosition(
		HudElementId,
		Vec2<float>(
			std::max(0.f, screenWidth - listWidth - 10.f),
			bottom ? std::max(0.f, screenHeight - listHeight - 10.f) : 10.f),
		Vec2<float>(screenWidth, screenHeight));

	const Vec2<float> normalizedPosition = HudEditor::getPosition(HudElementId);
	const float listX = std::clamp(
		normalizedPosition.x * screenWidth, 0.f, std::max(0.f, screenWidth - listWidth));
	const float listY = std::clamp(
		normalizedPosition.y * screenHeight, 0.f, std::max(0.f, screenHeight - listHeight));
	float currentY = bottom ? listY + listHeight : listY;
	const float baseX = listX + listWidth;

	for (std::size_t index = 0; index < renderCount; ++index) {
		const ArraylistEntry& entry = renderList[index];
		const float dynamicHeight = moduleHeight * entry.alpha;
		const float xRight = baseX + (1.f - entry.alpha) * entry.totalWidth;
		const float xLeft = xRight - entry.totalWidth;
		const float rowTop = bottom ? currentY - dynamicHeight : currentY;
		const float rowBottom = bottom ? currentY : currentY + dynamicHeight;
		const Vec4<float> rowRect = snapRect(
			Vec4<float>(xLeft, rowTop, xRight, rowBottom));
		currentY += bottom ? -dynamicHeight : dynamicHeight;

		const bool first = index == 0;
		const bool last = index + 1 == renderCount;
		const ImDrawFlags backgroundFlags = getRowFlags(rowRounding > 0.f, bottom, first, last);
		const ImDrawFlags accentFlags = getBarFlags(rowRounding > 0.f, bottom, first, last);
		const int backgroundAlpha = alphaByte(entry.alpha, 255.f * bgOpacity);

		ImGuiRender::drawBlur(rowRect, rowRounding, 0.72f * entry.alpha, backgroundFlags);
		drawlist->AddRectFilled(
			ImVec2(rowRect.x, rowRect.y), ImVec2(rowRect.z, rowRect.w),
			IM_COL32(12, 14, 18, backgroundAlpha), rowRounding, backgroundFlags);

		const Vec4<float> barRect = snapRect(
			Vec4<float>(rowRect.z - barWidth, rowRect.y, rowRect.z, rowRect.w));
		if (glow)
			drawGlowRect(
				drawlist, barRect, entry.color, entry.alpha,
				rowRounding, accentFlags, glowPower);
		drawlist->AddRectFilled(
			ImVec2(barRect.x, barRect.y), ImVec2(barRect.z, barRect.w),
			entry.color.toImColor(alphaByte(entry.alpha)), rowRounding, accentFlags);

		if (alphaByte(entry.alpha) == 0)
			continue;

		const float textY = rowRect.y + (moduleHeight - textHeight) * 0.5f;
		const float textX = rowRect.x + paddingX;
		const UIColor nameColor(
			textColor.r, textColor.g, textColor.b,
			static_cast<uint8_t>(alphaByte(entry.alpha, textColor.a)));
		const UIColor modeColor(235, 238, 242, static_cast<uint8_t>(alphaByte(entry.alpha)));

		drawlist->PushClipRect(ImVec2(0.f, rowRect.y), ImVec2(screenWidth, rowRect.w), true);
		if (glow && entry.alpha > 0.02f) {
			drawGlowText(
				Vec2<float>(textX, textY), entry.name,
				entry.color, textSize, entry.alpha, glowPower);
			if (!entry.modeText.empty()) {
				drawGlowText(
					Vec2<float>(textX + entry.nameWidth, textY), entry.modeText,
					UIColor(255, 255, 255), textSize, entry.alpha, glowPower * 0.7f);
			}
		}

		ImGuiRender::drawText(Vec2<float>(textX, textY), entry.name, nameColor, textSize);
		if (!entry.modeText.empty()) {
			ImGuiRender::drawText(
				Vec2<float>(textX + entry.nameWidth, textY), entry.modeText, modeColor, textSize);
		}
		drawlist->PopClipRect();
	}
}

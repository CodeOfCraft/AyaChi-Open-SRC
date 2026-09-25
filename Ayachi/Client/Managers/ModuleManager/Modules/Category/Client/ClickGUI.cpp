#include "ClickGUI.h"

#include <algorithm>
#include <cmath>

#include "../../../../../../Libs/json.hpp"
#include "../../../../../../Utils/TimerUtil.h"
#include "../../../../../Client.h"
#include "../../../../../Managers/ModuleManager/ModuleManager.h"
#include "../Client/Colors.h"
#include "../../../../../../Renderer/ImGuiRender.h"
#include <filesystem>
#include "../../../../ConfigManager/ConfigManager.h"
#include "../../../../../../Utils/FileUtil.h"


static inline float easeOutCubic(float t) {
    t = std::clamp(t, 0.f, 1.f);
    return 1.f - powf(1.f - t, 3.f);
}

static inline float easeOutBack(float t) {
    t = std::clamp(t, 0.f, 1.f);
    constexpr float c1 = 1.70158f;
    constexpr float c3 = c1 + 1.f;
    return 1.f + c3 * powf(t - 1.f, 3.f) + c1 * powf(t - 1.f, 2.f);
}

static inline Vec4<float> snapRectToPixels(const Vec4<float>& rect) {
    return Vec4<float>(
        floorf(rect.x),
        floorf(rect.y),
        ceilf(rect.z),
        ceilf(rect.w)
    );
}

static inline Vec4<float> snapAnimatedRectToPixels(const Vec4<float>& rect) {
    return snapRectToPixels(rect);
}

static inline Vec4<float> snapListItemRectToPixels(const Vec4<float>& rect) {
    Vec4<float> snapped(
        floorf(rect.x),
        roundf(rect.y),
        ceilf(rect.z),
        roundf(rect.w)
    );
    if (snapped.w <= snapped.y)
        snapped.w = snapped.y + 1.f;
    return snapped;
}

static inline int alphaByte(float alpha, float value = 255.f) {
    return (int)std::clamp(value * alpha, 0.f, 255.f);
}

static constexpr float clickGuiBlurAlpha = 0.88f;

static inline void drawEdgeGlow(ImDrawList* drawlist, const Vec4<float>& rect, float rounding, float alpha, float spread, ImDrawFlags flags = 0) {
    if (!drawlist || alpha <= 0.f || spread <= 0.f)
        return;

    const float glowSpread = spread * 1.35f;
    for (int i = 6; i >= 1; i--) {
        const float t = (float)i / 6.f;
        const float expand = glowSpread * t;
        const int glowAlpha = alphaByte(alpha, 18.f * (1.f - t * 0.55f));
        if (glowAlpha <= 0)
            continue;
        drawlist->AddRectFilled(
            ImVec2(rect.x - expand, rect.y - expand),
            ImVec2(rect.z + expand, rect.w + expand),
            IM_COL32(0, 0, 0, glowAlpha),
            rounding + expand,
            flags);
    }
}

static inline std::string fitTextToWidth(std::string text, float maxWidth, float textSize) {
    if (maxWidth <= 0.f)
        return "";
    if (ImGuiRender::getTextWidth(text, textSize) <= maxWidth)
        return text;

    const std::string prefix = "...";
    if (ImGuiRender::getTextWidth(prefix, textSize) > maxWidth)
        return "";
    while (!text.empty() && ImGuiRender::getTextWidth(prefix + text, textSize) > maxWidth)
        text.erase(text.begin());
    return prefix + text;
}

static inline void drawBottomTabIcon(ImDrawList* drawlist, int index, const Vec2<float>& center, UIColor color, float guiScale) {
    ImU32 col = color.toImColor();
    float s = guiScale;

    if (index == 0) {
		float box = 3.1f * s;
		float gap = 2.0f * s;
		for (int yy = 0; yy < 2; yy++) {
			for (int xx = 0; xx < 2; xx++) {
				float x = center.x - box - gap * 0.5f + xx * (box + gap);
				float y = center.y - box - gap * 0.5f + yy * (box + gap);
				drawlist->AddRectFilled(ImVec2(x, y), ImVec2(x + box, y + box), col, 1.2f * s);
			}
		}
        return;
    }

    if (index == 1) {
		for (int row = -1; row <= 1; ++row) {
			const float y = center.y + row * 4.f * s;
			drawlist->AddCircleFilled(ImVec2(center.x - 5.f * s, y), 1.f * s, col, 8);
			drawlist->AddLine(ImVec2(center.x - 2.f * s, y), ImVec2(center.x + 6.f * s, y), col, 1.2f * s);
		}
        return;
    }

    drawlist->AddCircle(ImVec2(center.x, center.y), 3.5f * s, col, 14, 1.2f * s);
    drawlist->AddCircleFilled(ImVec2(center.x, center.y), 1.2f * s, col, 10);
    for (int i = 0; i < 6; i++) {
        float a = (float)i / 6.f * 6.2831853f;
        Vec2<float> p1(center.x + cosf(a) * 5.2f * s, center.y + sinf(a) * 5.2f * s);
        Vec2<float> p2(center.x + cosf(a) * 6.8f * s, center.y + sinf(a) * 6.8f * s);
        drawlist->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), col, 1.1f * s);
    }
}

static inline float getSettingItemHeight(Setting* setting, float textHeight, float textPadding, float guiScale) {
    float itemHeight = textHeight + textPadding * 2.5f;
    if (setting->type == SettingType::SLIDER_S)
        itemHeight += 12.0f * guiScale;
    if (setting->type == SettingType::COLOR_S) {
        ColorSetting* cs = static_cast<ColorSetting*>(setting);
        if (cs->extended)
            itemHeight += 160.0f * guiScale;
    }
    return itemHeight;
}

ClickGUI::ClickGUI()
    : Module("ClickGUI", "NULL", Category::CLIENT, VK_INSERT) {
    scaleSetting = static_cast<SliderSetting<float>*>(
        registerSetting(new SliderSetting<float>("Scale", "ClickGUI scale", &scale, 1.15f, 0.8f, 1.6f)));
	fontSetting = static_cast<FontSetting*>(
		registerSetting(new FontSetting("Font", "Client interface font", &fontSelection)));
	fontSetting->dependOn = []() { return false; };
}

ClickGUI::~ClickGUI() {
    for (ClickWindow* window : windowList) {
        delete window;
    }
}

void ClickGUI::onEnable() {
    Mouse::refreshOwner();
    openDuration = 0.f;
    pageAnim = 0.f;
	activeBottomPage = 0;
    isSearching = false;
    searchingModule.clear();
    for (auto& module : ModuleManager::moduleList) {
        animProgress[module] = 0.0f;
        hoverAnim[module] = 0.0f;
    }
}

void ClickGUI::onDisable() {
    isSearching = false;
    searchingModule.clear();
    draggingSliderSettingPtr = nullptr;
    capturingKbSettingPtr = nullptr;
    for (ClickWindow* window : windowList)
        window->isDragging = false;

    Mouse::refreshOwner();
}

void getModuleListByCategory(Category category, std::vector<Module*>& modList) {
    for (Module* mod : ModuleManager::moduleList) {
        if (mod->getCategory() == category) {
            modList.push_back(mod);
        }
    }
}

void ClickGUI::onKeyUpdate(int key, bool isDown) {
    if (key == VK_SHIFT)
        isShiftDown = isDown;
    if (key == VK_BACK)
        isBackKeyDown = isDown;
    if (!isEnabled()) {
        if (key == getKeybind() && isDown) {
            setEnabled(true);
        }
    }
    else {
        if (isRenaming && isDown) {
            if (key == VK_RETURN) {
                if (!renameBuffer.empty() && renameBuffer != renameOldName) {
                    ConfigManager::renameConfig(renameOldName, renameBuffer);
                    refreshConfigPanelEntries();
                }
                isRenaming = false;
                return;
            }
            if (key == VK_ESCAPE) {
                isRenaming = false;
                return;
            }
            if (key == VK_BACK && !renameBuffer.empty()) {
                renameBuffer.pop_back();
                return;
            }
            static auto isValidCfg = [](char c) -> bool {
                if (c >= '0' && c <= '9') return true;
                if (c >= 'a' && c <= 'z') return true;
                if (c >= 'A' && c <= 'Z') return true;
                if (c == ' ' || c == '-' || c == '_' || c == '.') return true;
                return false;
            };
            if (renameBuffer.size() < 32 && isValidCfg((char)key)) {
                if (isShiftDown)
                    renameBuffer += (char)key;
                else
                    renameBuffer += (char)std::tolower(key);
            }
            return;
        }
        if (isDown) {
            if (key < 192) {
                if (capturingKbSettingPtr != nullptr) {
                    if (key != VK_ESCAPE)
                        *capturingKbSettingPtr->value = key;
                    capturingKbSettingPtr = nullptr;
                    return;
                }
            }
            if (key == getKeybind() || key == VK_ESCAPE) {
                setEnabled(false);
            }
            if (isSearching) {
                static auto isValid = [](char c) -> bool {
                    if (c >= '0' && c <= '9')
                        return true;
                    if (c >= 'a' && c <= 'z')
                        return true;
                    if (c >= 'A' && c <= 'Z')
                        return true;
                    return false;
                    };
                if (key == VK_BACK && !searchingModule.empty())
                    searchingModule.pop_back();
                else if (key == ' ')
                    searchingModule += ' ';
                else if (isValid((char)key)) {
                    if (isShiftDown)
                        searchingModule += (char)key;
                    else
                        searchingModule += std::tolower(key);
                }
            }
        }
    }
}

void ClickGUI::init() {
    setEnabled(false);

    ImGuiIO& io = ImGui::GetIO();
    const float guiScale = std::clamp(scale, 0.8f, 1.6f);
    lastScale = guiScale;
    const float windowWidth = 158.f * guiScale;
    const float windowGap = 12.f * guiScale;
    const float totalWidth = windowWidth * 7.f + windowGap * 6.f;
    Vec2<float> startPos = Vec2<float>(std::max(12.f * guiScale, (io.DisplaySize.x - totalWidth) * 0.5f), 60.f * guiScale);

    this->windowList.emplace_back(new ClickWindow(startPos, Category::COMBAT));
    startPos.x += windowWidth + windowGap;
    this->windowList.emplace_back(new ClickWindow(startPos, Category::MOVEMENT));
    startPos.x += windowWidth + windowGap;
    this->windowList.emplace_back(new ClickWindow(startPos, Category::RENDER));
    startPos.x += windowWidth + windowGap;
    this->windowList.emplace_back(new ClickWindow(startPos, Category::PLAYER));
    startPos.x += windowWidth + windowGap;
    this->windowList.emplace_back(new ClickWindow(startPos, Category::WORLD));
    startPos.x += windowWidth + windowGap;
    this->windowList.emplace_back(new ClickWindow(startPos, Category::MISC));
    startPos.x += windowWidth + windowGap;
    this->windowList.emplace_back(new ClickWindow(startPos, Category::CLIENT));
}

void ClickGUI::initSnow(float width, float height) {
    ImGuiIO& io = ImGui::GetIO();
    const float w = std::max(1.f, width > 0.f ? width : io.DisplaySize.x);
    const float h = std::max(1.f, height > 0.f ? height : io.DisplaySize.y);
    snowViewport = Vec2<float>(w, h);

    constexpr int snowCount = 480;
    snowflakes.resize(snowCount);

    const int iw = std::max(1, (int)w);
    const int ih = std::max(1, (int)h);
    for (auto& s : snowflakes) {
        const float r1 = (float)(rand() % 1000) / 1000.f;
        const float r2 = (float)(rand() % 1000) / 1000.f;
        const float r3 = (float)(rand() % 1000) / 1000.f;
        const float r4 = (float)(rand() % 1000) / 1000.f;
        s.x = (float)(rand() % iw);
        s.y = (float)(rand() % ih);
        s.size = 0.75f + r1 * 1.65f;
        s.speed = 20.f + r2 * 52.f;
        s.drift = 5.f + r3 * 17.f;
        s.opacity = 0.16f + r4 * 0.36f;
        s.phase = (float)(rand() % 628) / 100.f;
    }
}

void ClickGUI::renderBackground(ImDrawList* drawlist, const Vec2<float>& screenSize, UIColor accentColor, float alpha) {
    if (!drawlist || alpha <= 0.f || screenSize.x <= 1.f || screenSize.y <= 1.f)
        return;

    const float w = screenSize.x;
    const float h = screenSize.y;

    drawlist->AddRectFilled(
        ImVec2(0.f, 0.f), ImVec2(w, h),
        IM_COL32(3, 4, 8, alphaByte(alpha, 118.f)));

    drawlist->AddRectFilled(
        ImVec2(0.f, 0.f), ImVec2(w, h),
        IM_COL32((int)(accentColor.r * 0.08f), (int)(accentColor.g * 0.08f), (int)(accentColor.b * 0.11f), alphaByte(alpha, 28.f)));

    renderSnow(drawlist, screenSize, accentColor, alpha * 0.95f, 0.f);
}

void ClickGUI::renderSnow(ImDrawList* drawlist, const Vec2<float>& screenSize, UIColor accentColor, float alpha, float yStart) {
    if (!drawlist || alpha <= 0.f || screenSize.x <= 1.f || screenSize.y <= 1.f)
        return;

    yStart = std::clamp(yStart, 0.f, screenSize.y - 1.f);
    const float snowHeight = std::max(1.f, screenSize.y - yStart);
    if (!snowInitialized || snowflakes.size() != 480 || fabsf(snowViewport.x - screenSize.x) > 2.f || fabsf(snowViewport.y - snowHeight) > 2.f) {
        initSnow(screenSize.x, snowHeight);
        snowInitialized = true;
    }

    ImGuiIO& io = ImGui::GetIO();
    const float dt = std::clamp(io.DeltaTime, 0.f, 0.05f);
    const float w = screenSize.x;
    const float h = snowHeight;
    const int iw = std::max(1, (int)w);
    const float time = (float)ImGui::GetTime();

    drawlist->PushClipRect(ImVec2(0.f, yStart), ImVec2(screenSize.x, screenSize.y), true);
    for (auto& s : snowflakes) {
        s.y += s.speed * dt;
        s.x += sinf(time * 0.8f + s.phase) * s.drift * dt;
        if (s.y > h + 6.f) {
            s.y = -6.f;
            s.x = (float)(rand() % iw);
        }
        if (s.x < -6.f) s.x = w + 6.f;
        if (s.x > w + 6.f) s.x = -6.f;

        const float glowPulse = 0.62f + 0.38f * sinf(time * 2.2f + s.phase * 1.7f);
        const float drawY = yStart + s.y;
        const int haloAlpha = alphaByte(alpha, s.opacity * (24.f + 34.f * glowPulse));
        const int midAlpha = alphaByte(alpha, s.opacity * (38.f + 28.f * glowPulse));
        const int coreAlpha = alphaByte(alpha, s.opacity * 205.f);
        if (coreAlpha <= 0)
            continue;

        drawlist->AddCircleFilled(ImVec2(s.x, drawY), s.size * (3.1f + glowPulse * 1.25f), accentColor.toImColor(haloAlpha), 12);
        drawlist->AddCircleFilled(ImVec2(s.x, drawY), s.size * (2.0f + glowPulse * 0.55f), IM_COL32(190, 220, 255, midAlpha), 10);
        drawlist->AddCircleFilled(ImVec2(s.x, drawY), s.size, IM_COL32(248, 250, 255, coreAlpha), 8);
    }
    drawlist->PopClipRect();
}

void ClickGUI::render(ImDrawList* drawlist) {
	if (fontSetting != nullptr)
		fontSetting->update();
    if (!initClickGUI) {
        ClickGUI::init();
        initClickGUI = true;
    }
    if (!snowInitialized) {
        initSnow();
        snowInitialized = true;
    }
    if (!this->isEnabled()) {
        openDuration = 0.f;
        return;
    }
    const Vec2<float>& mousePos = Mouse::pos();

    static Colors* colorsMod = ModuleManager::getModule<Colors>();
    ImGuiIO& io = ImGui::GetIO();

    const float targetScale = std::clamp(scale, 0.8f, 1.6f);
    const bool draggingScale = draggingSliderSettingPtr == scaleSetting;
    const float guiScale = draggingScale ? std::clamp(lastScale, 0.8f, 1.6f) : targetScale;
    textSize = 0.78f * guiScale;
    if (!draggingScale && fabsf(lastScale - guiScale) > 0.001f) {
        const float ratio = guiScale / std::max(lastScale, 0.01f);
        const float centerX = io.DisplaySize.x * 0.5f;
        for (ClickWindow* window : windowList) {
            window->pos.x = centerX + (window->pos.x - centerX) * ratio;
            window->pos.y *= ratio;
        }
        lastScale = guiScale;
    }

    const float windowWidth = 158.f * guiScale;
    const float textHeight = ImGuiRender::getTextHeight(textSize);
    const float textPadding = 2.75f * guiScale;
    float roundValue = 9.0f * guiScale;
    const float settingsTopGap = 2.5f * guiScale;

    static const UIColor whiteColor(255, 255, 255, 255);
    static const UIColor grayColor(190, 190, 196, 255);
    static const UIColor darkGrayColor(40, 40, 40, 255);
    static const UIColor settingBgColor(12, 12, 14, 155);

    float targetDuration = 1.f;
    openDuration = Math::lerp(openDuration, targetDuration, io.DeltaTime * 5.f);

    if (openDuration > 0.99f)
        openDuration = 1.f;

    float animEase = easeOutCubic(openDuration);
    pageAnim = Math::lerp(pageAnim, 1.f, std::clamp(io.DeltaTime * 10.f, 0.f, 1.f));
    if (pageAnim > 0.99f)
        pageAnim = 1.f;
    float pageEase = easeOutCubic(pageAnim);

    UIColor accentColor(colorsMod->getColor());

    std::string tooltipString = "NULL";
    if (Mouse::leftClicked() || Mouse::rightClicked())
        isSearching = false;

    {
        pageEase = easeOutCubic(pageAnim);

        const Vec2<float> screenSize(io.DisplaySize.x, io.DisplaySize.y);
        renderBackground(drawlist, screenSize, accentColor, animEase);
        renderBottomTabs(screenSize, animEase);

        const float wheelDelta = Mouse::takeWheel();
        if (wheelDelta != 0.f) {
            const int wheelSteps = (int)(wheelDelta > 0.f ? ceilf(wheelDelta) : floorf(wheelDelta));
			if (activeBottomPage == 1)
                configPanelScroll = std::max(0, configPanelScroll - wheelSteps);
			else if (activeBottomPage == 2)
				settingsPanelScroll = std::max(0, settingsPanelScroll - wheelSteps);
            else {
                for (ClickWindow* window : windowList)
                    window->pos.y += wheelDelta * 15.f;
            }
        }

		if (activeBottomPage == 1) {
			renderConfigPanel(Vec2<float>(io.DisplaySize.x, io.DisplaySize.y), animEase * pageEase);
            Mouse::consumeLeft();
            Mouse::consumeRight();
            return;
        }
        if (activeBottomPage == 2) {
			renderSettingsPanel(Vec2<float>(io.DisplaySize.x, io.DisplaySize.y), animEase * pageEase);
            Mouse::consumeLeft();
            Mouse::consumeRight();
            return;
        }

        for (size_t windowIndex = 0; windowIndex < windowList.size(); windowIndex++) {
            ClickWindow* window = windowList[windowIndex];
            if (window->pos.x > io.DisplaySize.x)
                window->pos.x = io.DisplaySize.x - windowWidth;
            if (window->pos.y > io.DisplaySize.y)
                window->pos.y = io.DisplaySize.y - 200.f * guiScale;

            if (window->isDragging) {
                window->pos.x += mousePos.x - startDragPos.x;
                window->pos.y += mousePos.y - startDragPos.y;
                startDragPos = mousePos;
                if (!Mouse::leftHeld())
                    window->isDragging = false;
            }

            window->extendedAnim = Math::lerp(window->extendedAnim, window->extended ? 1.f : 0.f,
                std::clamp(io.DeltaTime * 14.f, 0.f, 1.f));
            if (window->extendedAnim > 0.99f)
                window->extendedAnim = 1.f;
            if (window->extendedAnim < 0.01f && !window->extended)
                window->extendedAnim = 0.f;

            float bodyAnim = easeOutCubic(window->extendedAnim);
            bool bodyVisible = bodyAnim > 0.01f;
            float windowT = std::clamp(animEase * 1.25f - (float)windowIndex * 0.045f, 0.f, 1.f);
            float windowEase = easeOutBack(windowT) * pageEase;

            std::vector<Module*> visibleModules;
            if (window->extended || bodyVisible) {
                for (Module* mod : window->modList) {
                    if (!searchingModule.empty()) {
                        std::string lowerSearch = searchingModule;
						std::string lowerModule = mod->getModuleName() + " " + mod->getDisplayName();
                        std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(),
                            ::tolower);
                        std::transform(lowerModule.begin(), lowerModule.end(), lowerModule.begin(),
                            ::tolower);
                        if (lowerModule.find(lowerSearch) == std::string::npos)
                            continue;
                    }
                    visibleModules.push_back(mod);
                }
            }

            float headerH = textHeight + textPadding * 2.4f;
            float introX = 0.f;
            float introY = (1.f - windowEase) * 12.f * guiScale;
            Vec4<float> headerRect = Vec4<float>(
                window->pos.x + introX, window->pos.y - introY,
                window->pos.x + introX + windowWidth, window->pos.y - introY + headerH);
            headerRect = snapRectToPixels(headerRect);

            const bool hasBody = bodyVisible && !visibleModules.empty();

            float listBottom = headerRect.w;
            if (hasBody) {
                for (Module* mod : visibleModules) {
                    mod->selectedAnim = Math::lerp(mod->selectedAnim, mod->extended ? 1.f : 0.f,
                        std::clamp(io.DeltaTime * 14.f, 0.f, 1.f));
                    if (mod->selectedAnim > 0.99f)
                        mod->selectedAnim = 1.f;
                    if (mod->selectedAnim < 0.01f && !mod->extended)
                        mod->selectedAnim = 0.f;
                }

                for (Module* mod : visibleModules) {
                    float modH = textHeight + textPadding * 2.2f;
                    float visibleModH = modH * bodyAnim;
                    if (visibleModH <= 1.f)
                        continue;

                    listBottom += visibleModH;

                    float settingsAnim = easeOutCubic(mod->selectedAnim) * bodyAnim;
                    if ((mod->extended || mod->selectedAnim > 0.01f) && settingsAnim > 0.01f) {
                        bool hasVisibleSettings = false;
                        for (auto& setting : mod->getSettingList()) {
                            if (setting->isVisible()) {
                                if (!hasVisibleSettings) {
                                    listBottom += settingsTopGap * settingsAnim;
                                    hasVisibleSettings = true;
                                }
                                listBottom += getSettingItemHeight(setting, textHeight, textPadding, guiScale) * settingsAnim;
                            }
                        }
                    }
                }
            }

            Vec4<float> panelRect = hasBody
                ? snapAnimatedRectToPixels(Vec4<float>(headerRect.x, headerRect.y, headerRect.z, listBottom))
                : headerRect;
            ImDrawFlags headerFlags = hasBody ? ImDrawFlags_RoundCornersTop : ImDrawFlags_RoundCornersAll;
            const bool headerHovered = headerRect.contains(mousePos);
            const float headerGlowAlpha = (hasBody ? 0.72f : 0.58f)
                * (headerHovered ? 1.18f : 1.f)
                * (window->isDragging ? 1.28f : 1.f)
                * animEase * pageEase;

            drawEdgeGlow(drawlist, panelRect, roundValue, headerGlowAlpha, 8.f * guiScale, ImDrawFlags_RoundCornersAll);
            ImGuiRender::drawBlur(panelRect, roundValue, clickGuiBlurAlpha, ImDrawFlags_RoundCornersAll);
            drawlist->AddRectFilled(ImVec2(panelRect.x, panelRect.y),
                ImVec2(panelRect.z, panelRect.w), IM_COL32(10, 10, 12, 132),
                roundValue, ImDrawFlags_RoundCornersAll);

            ImU32 headerOutline = IM_COL32(255, 255, 255, headerHovered ? 24 : 16);
            drawlist->AddRect(ImVec2(panelRect.x, panelRect.y),
                ImVec2(panelRect.z, panelRect.w), headerOutline,
                roundValue, ImDrawFlags_RoundCornersAll, 1.0f);

            drawlist->AddRectFilledMultiColor(
                ImVec2(headerRect.x + 1.f, headerRect.y + 1.f),
                ImVec2(headerRect.z - 1.f, headerRect.w),
                IM_COL32(255, 255, 255, headerHovered ? 11 : 7),
                IM_COL32(255, 255, 255, headerHovered ? 11 : 7),
                IM_COL32(255, 255, 255, 0),
                IM_COL32(255, 255, 255, 0));

            Vec2<float> titlePos = Vec2<float>(headerRect.x + 7.f * guiScale,
                headerRect.y + (headerH - textHeight) * 0.5f - 1.f * guiScale);
			ImGuiRender::drawText(titlePos,
				window->windowName,
				whiteColor, textSize, true);

            if (headerHovered) {
                if (Mouse::leftClicked() && !window->isDragging) {
                    window->isDragging = true;
                    startDragPos = mousePos;
                    Mouse::consumeLeft();
                }
                else if (Mouse::rightClicked()) {
                    window->extended = !window->extended;
                    Mouse::consumeRight();
                }
            }

            float xStart = headerRect.x;
            float xEnd = headerRect.z;
            float yOffset = headerRect.w;

            if (bodyVisible && !visibleModules.empty()) {
                Vec4<float> listBgRect = snapAnimatedRectToPixels(Vec4<float>(xStart, headerRect.w, xEnd, listBottom));

                int modIndex = 0;
                for (size_t i = 0; i < visibleModules.size(); i++) {
                    Module* mod = visibleModules[i];
                    bool isLastModule =
                        (i == visibleModules.size() - 1);

                    UIColor modColor = colorsMod->getColor(-modIndex);
                    modIndex += colorsMod->getSeperation();

                    float modH = textHeight + textPadding * 2.2f;
                    float visibleModH = modH * bodyAnim;
                    Vec4<float> modRect = { xStart, yOffset, xEnd, yOffset + visibleModH };
                    if (visibleModH <= 1.f)
                        continue;

                    float settingsAnim = easeOutCubic(mod->selectedAnim) * bodyAnim;
                    std::vector<Setting*> visibleSettings;
                    if ((mod->extended || mod->selectedAnim > 0.01f) && settingsAnim > 0.01f) {
                        for (auto& s : mod->getSettingList()) {
                            if (s->isVisible())
                                visibleSettings.push_back(s);
                        }
                    }
                    const bool showSettings = !visibleSettings.empty();

                    Vec4<float> modDrawRect = snapListItemRectToPixels(modRect);
                    drawlist->PushClipRect(ImVec2(modDrawRect.x, modDrawRect.y), ImVec2(modDrawRect.z, modDrawRect.w), true);

                    float& progress = animProgress[mod];
                    float targetProgress = mod->isEnabled() ? 1.0f : 0.0f;
                    progress = Math::lerp(progress, targetProgress, std::clamp(io.DeltaTime * 25.f, 0.f, 1.f));

                    bool isHovered = modRect.contains(mousePos);
 
                    float& hover = hoverAnim[mod];
                    float targetHover = isHovered ? 1.0f : 0.0f;
                    hover = Math::lerp(hover, targetHover, std::clamp(io.DeltaTime * 15.f, 0.f, 1.f));

                    float modRound = 0.0f;
                    ImDrawFlags modFlags = 0;
                    if (isLastModule && !showSettings) {
                        modRound = roundValue;
                        modFlags = ImDrawFlags_RoundCornersBottom;
                    }

                    int bgShade = (int)(18.f + 10.f * hover);
                    int bgA = (int)(24.f * hover);
                    ImU32 cellBg = IM_COL32(bgShade, bgShade, bgShade + 2, bgA);
                    drawlist->AddRectFilled(ImVec2(modDrawRect.x, modDrawRect.y),
                        ImVec2(modDrawRect.z, modDrawRect.w), cellBg, modRound,
                        modFlags);

                    if (progress > 0.01f) {
                        int alpha = (int)((72.f + 24.f * hover) * progress);
                        drawlist->AddRectFilled(ImVec2(modDrawRect.x, modDrawRect.y),
                            ImVec2(modDrawRect.z, modDrawRect.w),
                            modColor.toImColor(alpha), modRound, modFlags);
                    }

                    Vec2<float> textPos = { modDrawRect.x + 7.f * guiScale,
                                           modDrawRect.y + (modH - textHeight) * 0.5f - 1.f * guiScale };
                    UIColor txtColor = (mod->isEnabled() || isHovered) ? whiteColor : grayColor;
					ImGuiRender::drawText(textPos, mod->getDisplayName(), txtColor, textSize, true);

                    std::string expandMark = mod->extended ? "-" : "+";
                    float markWidth = ImGuiRender::getTextWidth(expandMark, textSize);
                    ImGuiRender::drawText(
                        Vec2<float>(modDrawRect.z - markWidth - 7.f * guiScale, textPos.y),
                        expandMark, txtColor, textSize, true);
                    drawlist->PopClipRect();

                    if (isHovered) {
                        tooltipString = mod->getDescription();
                        if (Mouse::leftClicked()) {
                            mod->toggle();
                            Mouse::consumeLeft();
                        }
                        else if (Mouse::rightClicked()) {
                            mod->extended = !mod->extended;
                            Mouse::consumeRight();
                        }
                    }

                    yOffset += visibleModH;

                    if (showSettings) {
                        float visibleSettingsTopGap = settingsTopGap * settingsAnim;
                        if (visibleSettingsTopGap > 0.5f) {
                            Vec4<float> gapRect = snapRectToPixels(Vec4<float>(xStart, yOffset, xEnd, yOffset + visibleSettingsTopGap));
                            Setting* firstSetting = visibleSettings.front();
                            const int bridgeBottomAlpha = (int)(42.f + 34.f * firstSetting->selectedAnim);
                            const int bridgeTopAlpha = (int)(bridgeBottomAlpha * 0.18f);
                            drawlist->AddRectFilledMultiColor(
                                ImVec2(gapRect.x, gapRect.y),
                                ImVec2(gapRect.z, gapRect.w),
                                IM_COL32(12, 12, 14, bridgeTopAlpha),
                                IM_COL32(12, 12, 14, bridgeTopAlpha),
                                IM_COL32(12, 12, 14, bridgeBottomAlpha),
                                IM_COL32(12, 12, 14, bridgeBottomAlpha));
                        }
                        yOffset += visibleSettingsTopGap;

                        for (size_t j = 0; j < visibleSettings.size(); j++) {
                            Setting* setting = visibleSettings[j];
                            bool isLastSetting = (j == visibleSettings.size() - 1);

                            float itemHeight = getSettingItemHeight(setting, textHeight, textPadding, guiScale);

                            float visibleItemHeight = itemHeight * settingsAnim;
                            Vec4<float> setRect = { xStart, yOffset, xEnd, yOffset + visibleItemHeight };
                            if (visibleItemHeight <= 1.f)
                                continue;
                            setRect = snapListItemRectToPixels(setRect);
                            drawlist->PushClipRect(ImVec2(setRect.x, setRect.y), ImVec2(setRect.z, setRect.w), true);

                            float setRound = 0.0f;
                            ImDrawFlags setFlags = 0;
                            if (isLastModule && isLastSetting) {
                                setRound = roundValue;
                                setFlags = ImDrawFlags_RoundCornersBottom;
                            }

                            Vec4<float> contentRect = { setRect.x + 12.f * guiScale, setRect.y, setRect.z - 7.f * guiScale,
                                                       setRect.w };
                            Vec2<float> sTextPos = { contentRect.x, contentRect.y + textPadding + 1.f * guiScale };

                            bool setHovered = contentRect.contains(mousePos);
                            if (setHovered)
								tooltipString = setting->getDescription();
                            setting->selectedAnim = Math::lerp(setting->selectedAnim,
                                (setHovered || setting->isDragging) ? 1.f : 0.f,
                                std::clamp(io.DeltaTime * 12.f, 0.f, 1.f));
                            int setShade = (int)(12.f + 12.f * setting->selectedAnim);
                            UIColor setCellBg(setShade, setShade, setShade,
                                (int)(42.f + 34.f * setting->selectedAnim));
                            drawlist->AddRectFilled(ImVec2(setRect.x, setRect.y),
                                ImVec2(setRect.z, setRect.w),
                                setCellBg.toImColor(), setRound, setFlags);

                            UIColor sColor = colorsMod->getColor(-modIndex);
                            modIndex += 15;


                            switch (setting->type) {
                            case(SettingType::BOOL_S): {
                                BoolSetting* bs = static_cast<BoolSetting*>(setting);
                                bool val = *bs->value;
                                drawlist->PushClipRect(ImVec2(contentRect.x, contentRect.y), ImVec2(contentRect.z - 35.f * guiScale, contentRect.w));
								ImGuiRender::drawText(sTextPos, setting->getDisplayName(), whiteColor,
                                    textSize, true);
                                drawlist->PopClipRect();

                                float switchW = 30.f * guiScale;
                                float switchH = 16.f * guiScale;
                                float midY = setRect.y + (itemHeight - switchH) / 2.f;
                                Vec4<float> switchRect = { contentRect.z - switchW, midY,
                                                          contentRect.z, midY + switchH };

                                UIColor toggleBg = val ? sColor : UIColor(60, 60, 60, 255);
                                ImGuiRender::fillRectangle(switchRect, toggleBg, 8.f * guiScale);

                                float circleX = val ? (switchRect.z - switchH / 2.f)
                                    : (switchRect.x + switchH / 2.f);
                                drawlist->AddCircleFilled(
                                    ImVec2(circleX, switchRect.y + switchH / 2.f),
                                    switchH / 2.f - 2.f * guiScale, IM_COL32(255, 255, 255, 255));

                                if (contentRect.contains(mousePos) && Mouse::leftClicked()) {
                                    *bs->value = !val;
                                    Mouse::consumeLeft();
                                }
                                break;
                            }
                            case(SettingType::SLIDER_S): {
                                SliderSettingBase* ssBase =
                                    static_cast<SliderSettingBase*>(setting);
                                if (!ssBase)
                                    break;

                                drawlist->PushClipRect(ImVec2(contentRect.x, contentRect.y), ImVec2(contentRect.z, contentRect.w));
								ImGuiRender::drawText(sTextPos, setting->getDisplayName(), whiteColor,
                                    textSize, true);
                                drawlist->PopClipRect();

                                std::string valStr;
                                float percent = 0.f;
                                if (ssBase->valueType == ValueType::INT_T) {
                                    SliderSetting<int>* si =
                                        static_cast<SliderSetting<int>*>(ssBase);
                                    if (si && si->valuePtr) {
                                        valStr = std::to_string(*si->valuePtr);
                                        percent = (float)(*si->valuePtr - si->minValue) /
                                            (float)(si->maxValue - si->minValue);
                                    }
                                }
                                else {
                                    SliderSetting<float>* sf =
                                        static_cast<SliderSetting<float>*>(ssBase);
                                    if (sf && sf->valuePtr) {
                                        char buf[32];
                                        sprintf_s(buf, "%.2f", *sf->valuePtr);
                                        valStr = buf;
                                        percent = (*sf->valuePtr - sf->minValue) /
                                            (sf->maxValue - sf->minValue);
                                    }
                                }
                                percent = std::clamp(percent, 0.f, 1.f);

                                float valWidth = ImGuiRender::getTextWidth(valStr, textSize);
                                ImGuiRender::drawText(
                                    Vec2<float>(contentRect.z - valWidth, sTextPos.y), valStr,
                                    grayColor, textSize, true);

                                float sliderTop = sTextPos.y + textHeight + 5.f * guiScale;
                                Vec4<float> sliderRect = { contentRect.x, sliderTop,
                                                          contentRect.z, sliderTop + 5.f * guiScale };

                                if (contentRect.contains(mousePos) && Mouse::leftClicked()) {
                                    setting->isDragging = true;
                                    draggingSliderSettingPtr = ssBase;
                                    Mouse::consumeLeft();
                                }
                                if (setting->isDragging) {
                                    if (!Mouse::leftHeld()) {
                                        setting->isDragging = false;
                                        draggingSliderSettingPtr = nullptr;
                                    }
                                    else {
                                        float newP =
                                            std::clamp((mousePos.x - sliderRect.x) /
                                                (sliderRect.z - sliderRect.x),
                                                0.f, 1.f);
                                        percent = newP;
                                        if (ssBase->valueType == ValueType::INT_T) {
                                            SliderSetting<int>* si =
                                                static_cast<SliderSetting<int>*>(ssBase);
                                            if (si)
                                                *si->valuePtr =
                                                (int)(si->minValue +
                                                    (si->maxValue - si->minValue) * newP);
                                        }
                                        else {
                                            SliderSetting<float>* sf =
                                                static_cast<SliderSetting<float>*>(ssBase);
                                            if (sf)
                                                *sf->valuePtr =
                                                sf->minValue +
                                                (sf->maxValue - sf->minValue) * newP;
                                        }
                                    }
                                }

                                ImGuiRender::fillRectangle(sliderRect, UIColor(60, 60, 60, 255),
                                    3.5f * guiScale);
                                Vec4<float> fillRect = {
                                    sliderRect.x, sliderRect.y,
                                    sliderRect.x + (sliderRect.z - sliderRect.x) * percent,
                                    sliderRect.w };
                                ImGuiRender::fillRectangle(fillRect, sColor, 3.5f * guiScale);
                                drawlist->AddCircleFilled(
                                    ImVec2(fillRect.z, (sliderRect.y + sliderRect.w) / 2.f),
                                    6.f * guiScale, IM_COL32(255, 255, 255, 255));
                                break;
                            }
                            case(SettingType::ENUM_S): {
                                EnumSetting* es = static_cast<EnumSetting*>(setting);
                                {
                                    float modeW = 0;
                                    if (es->value && *es->value >= 0 &&
                                        *es->value < es->enumList.size())
										modeW = ImGuiRender::getTextWidth(es->getDisplayValue(*es->value), textSize);
                                    drawlist->PushClipRect(ImVec2(contentRect.x, contentRect.y), ImVec2(contentRect.z - modeW - 6.f * guiScale, contentRect.w));
									ImGuiRender::drawText(sTextPos, setting->getDisplayName() + ":", whiteColor,
                                        textSize, true);
                                    drawlist->PopClipRect();
                                }

                                if (es->value && *es->value >= 0 &&
                                    *es->value < es->enumList.size()) {
									std::string modeName = es->getDisplayValue(*es->value);
                                    ImGuiRender::drawText(
                                        Vec2<float>(contentRect.z - ImGuiRender::getTextWidth(modeName, textSize), sTextPos.y),
                                        modeName, grayColor, textSize, true);
                                }

                                if (contentRect.contains(mousePos)) {
                                    if (Mouse::leftClicked()) {
                                        (*es->value)++;
                                        if (*es->value >= es->enumList.size())
                                            *es->value = 0;
                                        Mouse::consumeLeft();
                                    }
                                    else if (Mouse::rightClicked()) {
                                        (*es->value)--;
                                        if (*es->value < 0)
                                            *es->value = (int)es->enumList.size() - 1;
                                        Mouse::consumeRight();
                                    }
                                }
                                break;
                            }
                            case(SettingType::COLOR_S): {
                                ColorSetting* cs = static_cast<ColorSetting*>(setting);

                                drawlist->PushClipRect(ImVec2(contentRect.x, contentRect.y), ImVec2(contentRect.z - 30.f * guiScale, contentRect.w));
								ImGuiRender::drawText(sTextPos, setting->getDisplayName() + ":", whiteColor,
                                    textSize, true);
                                drawlist->PopClipRect();

                                float headerH = textHeight + textPadding * 2.5f;
                                Vec4<float> previewRect = {
                                    contentRect.z - 25.f * guiScale, setRect.y + (headerH - 12.f * guiScale) / 2.f,
                                    contentRect.z - 5.f * guiScale, setRect.y + (headerH + 12.f * guiScale) / 2.f };

                                ImGuiRender::fillRectangle(previewRect, *cs->colorPtr, 5.5f * guiScale);
                                drawlist->AddRect(ImVec2(previewRect.x, previewRect.y),
                                    ImVec2(previewRect.z, previewRect.w),
                                    IM_COL32(80, 80, 80, 255), 5.5f * guiScale);

                                if (previewRect.contains(mousePos) && Mouse::leftClicked()) {
                                    cs->extended = !cs->extended;
                                    Mouse::consumeLeft();
                                }

                                if (cs->extended) {
                                    float currentY = setRect.y + headerH + 6.f * guiScale;

                                    float toggleH = 22.f * guiScale;
                                    Vec4<float> toggleRowRect = { contentRect.x, currentY,
                                                                 contentRect.z,
                                                                 currentY + toggleH };

                                    float centerY = toggleRowRect.y + toggleH / 2.f;

                                    float textY = centerY - textHeight / 2.f;
                                    ImGuiRender::drawText(Vec2<float>(toggleRowRect.x, textY),
										"Theme", whiteColor, textSize);

                                    float switchW = 30.f * guiScale;
                                    float switchH = 14.f * guiScale;
                                    float switchY = centerY - switchH / 2.f;

                                    Vec4<float> sRect = { toggleRowRect.z - switchW, switchY,
                                                         toggleRowRect.z, switchY + switchH };

                                    bool themeActive = cs->colorSynced;

                                    UIColor toggleBg =
                                        themeActive ? accentColor : UIColor(60, 60, 60, 255);
                                    ImGuiRender::fillRectangle(sRect, toggleBg, 8.f * guiScale);

                                    float circleSize = switchH - 4.f * guiScale;
                                    float circleX = themeActive ? (sRect.z - switchH / 2.f)
                                        : (sRect.x + switchH / 2.f);
                                    drawlist->AddCircleFilled(
                                        ImVec2(circleX, switchY + switchH / 2.f),
                                        circleSize / 2.f, IM_COL32(255, 255, 255, 255));

                                    if (toggleRowRect.contains(mousePos) && Mouse::leftClicked()) {
                                        cs->colorSynced = !cs->colorSynced;
                                        Mouse::consumeLeft();
                                    }

                                    currentY += toggleH + 8.f * guiScale;

                                    float pickerHeight = 100.f * guiScale;

                                    Vec4<float> pickerArea = { contentRect.x, currentY,
                                                              contentRect.z,
                                                              currentY + pickerHeight };

                                    float barWidth = 12.f * guiScale;
                                    float spacing = 6.f * guiScale;
                                    Vec4<float> alphaRect = { contentRect.z - barWidth, currentY,
                                                             contentRect.z,
                                                             currentY + pickerHeight };
                                    Vec4<float> hueRect = { alphaRect.x - spacing - barWidth,
                                                           currentY, alphaRect.x - spacing,
                                                           currentY + pickerHeight };
                                    Vec4<float> svRect = { contentRect.x, currentY,
                                                          hueRect.x - spacing,
                                                          currentY + pickerHeight };

                                    float r = cs->colorPtr->r / 255.f;
                                    float g = cs->colorPtr->g / 255.f;
                                    float b = cs->colorPtr->b / 255.f;
                                    float h, s, v;
                                    ImGui::ColorConvertRGBtoHSV(r, g, b, h, s, v);

                                    static ColorSetting* activeCs = nullptr;
                                    static int dragMode = 0;
                                    if (!Mouse::leftHeld()) {
                                        activeCs = nullptr;
                                        dragMode = 0;
                                    }

                                    if (Mouse::leftClicked()) {
                                        if (svRect.contains(mousePos)) {
                                            activeCs = cs;
                                            dragMode = 1;
                                            Mouse::consumeLeft();
                                        }
                                        else if (hueRect.contains(mousePos)) {
                                            activeCs = cs;
                                            dragMode = 2;
                                            Mouse::consumeLeft();
                                        }
                                        else if (alphaRect.contains(mousePos)) {
                                            activeCs = cs;
                                            dragMode = 3;
                                            Mouse::consumeLeft();
                                        }
                                    }

                                    if (activeCs == cs && Mouse::leftHeld()) {
                                        if (dragMode == 1) {
                                            s = std::clamp(
                                                (mousePos.x - svRect.x) / (svRect.z - svRect.x),
                                                0.f, 1.f);
                                            v = std::clamp(1.f - (mousePos.y - svRect.y) /
                                                (svRect.w - svRect.y),
                                                0.f, 1.f);
                                        }
                                        else if (dragMode == 2) {
                                            h = std::clamp((mousePos.y - hueRect.y) /
                                                (hueRect.w - hueRect.y),
                                                0.f, 1.f);
                                        }
                                        else if (dragMode == 3) {
                                            float newA =
                                                1.f -
                                                std::clamp((mousePos.y - alphaRect.y) /
                                                    (alphaRect.w - alphaRect.y),
                                                    0.f, 1.f);
                                            cs->colorPtr->a = (int)(newA * 255.f);
                                        }

                                        if (dragMode != 3) {
                                            ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
                                            cs->colorPtr->r = (int)(r * 255.f);
                                            cs->colorPtr->g = (int)(g * 255.f);
                                            cs->colorPtr->b = (int)(b * 255.f);
                                        }
                                    }


                                    float hR, hG, hB;
                                    ImGui::ColorConvertHSVtoRGB(h, 1.f, 1.f, hR, hG, hB);
                                    ImU32 hueC = IM_COL32((int)(hR * 255), (int)(hG * 255),
                                        (int)(hB * 255), 255);
                                    drawlist->AddRectFilledMultiColor(
                                        ImVec2(svRect.x, svRect.y), ImVec2(svRect.z, svRect.w),
                                        IM_COL32(255, 255, 255, 255), hueC, hueC,
                                        IM_COL32(255, 255, 255, 255));
                                    drawlist->AddRectFilledMultiColor(
                                        ImVec2(svRect.x, svRect.y), ImVec2(svRect.z, svRect.w),
                                        0, 0, IM_COL32(0, 0, 0, 255), IM_COL32(0, 0, 0, 255));

                                    float dotX = svRect.x + s * (svRect.z - svRect.x);
                                    float dotY = svRect.y + (1.f - v) * (svRect.w - svRect.y);
                                    drawlist->AddCircle(ImVec2(dotX, dotY), 3.f * guiScale,
                                        IM_COL32(255, 255, 255, 255));
                                    drawlist->AddCircle(ImVec2(dotX, dotY), 4.f * guiScale,
                                        IM_COL32(0, 0, 0, 255));

                                    static const ImU32 hC[] = {
                                        IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255),
                                        IM_COL32(0, 255, 0, 255), IM_COL32(0, 255, 255, 255),
                                        IM_COL32(0, 0, 255, 255), IM_COL32(255, 0, 255, 255),
                                        IM_COL32(255, 0, 0, 255) };
                                    for (int i = 0; i < 6; i++) {
                                        drawlist->AddRectFilledMultiColor(
                                            ImVec2(hueRect.x,
                                                hueRect.y +
                                                (i * (hueRect.w - hueRect.y) / 6.f)),
                                            ImVec2(hueRect.z,
                                                hueRect.y + ((i + 1) *
                                                    (hueRect.w - hueRect.y) / 6.f)),
                                            hC[i], hC[i], hC[i + 1], hC[i + 1]);
                                    }
                                    float hy = hueRect.y + h * (hueRect.w - hueRect.y);
                                    ImGuiRender::fillRectangle(
                                        Vec4<float>(hueRect.x, hy - 1.f * guiScale, hueRect.z, hy + 1.f * guiScale),
                                        whiteColor);

                                    drawlist->AddRectFilled(ImVec2(alphaRect.x, alphaRect.y),
                                        ImVec2(alphaRect.z, alphaRect.w),
                                        IM_COL32(50, 50, 50, 255));

                                    ImU32 cT = IM_COL32((int)(r * 255), (int)(g * 255),
                                        (int)(b * 255), 255);
                                    ImU32 cB = IM_COL32((int)(r * 255), (int)(g * 255),
                                        (int)(b * 255), 0);
                                    drawlist->AddRectFilledMultiColor(
                                        ImVec2(alphaRect.x, alphaRect.y),
                                        ImVec2(alphaRect.z, alphaRect.w), cT, cT, cB, cB);

                                    float ay = alphaRect.y + (1.f - cs->colorPtr->a / 255.f) *
                                        (alphaRect.w - alphaRect.y);
                                    ImGuiRender::fillRectangle(
                                        Vec4<float>(alphaRect.x, ay - 1.f * guiScale, alphaRect.z, ay + 1.f * guiScale),
                                        whiteColor);
                                }
                                break;
                            }
                            default: {
                                {
                                    float keyW = 0;
                                    if (setting->type == SettingType::KEYBIND_S) {
                                        KeybindSetting* kb = static_cast<KeybindSetting*>(setting);
                                        std::string kn =
											(*kb->value == 0) ? "None" : KeyNames[*kb->value];
                                        if (setting == capturingKbSettingPtr)
                                            kn = "...";
                                        keyW = ImGuiRender::getTextWidth(kn, textSize);
                                    }
                                    drawlist->PushClipRect(ImVec2(contentRect.x, contentRect.y), ImVec2(contentRect.z - keyW - 6.f * guiScale, contentRect.w));
									ImGuiRender::drawText(sTextPos, setting->getDisplayName(), whiteColor,
                                        textSize, true);
                                    drawlist->PopClipRect();
                                }
                                if (setting->type == SettingType::KEYBIND_S) {
                                    KeybindSetting* kb = static_cast<KeybindSetting*>(setting);
                                    std::string kn =
										(*kb->value == 0) ? "None" : KeyNames[*kb->value];
                                    if (setting == capturingKbSettingPtr)
                                        kn = "...";
                                    ImGuiRender::drawText(
                                        Vec2<float>(contentRect.z -
                                            ImGuiRender::getTextWidth(kn, textSize),
                                            sTextPos.y),
                                        kn, grayColor, textSize, true);
                                    if (contentRect.contains(mousePos)) {
                                        if (Mouse::leftClicked()) {
                                            capturingKbSettingPtr =
                                                (capturingKbSettingPtr == kb) ? nullptr : kb;
                                            Mouse::consumeLeft();
                                        }
                                        else if (Mouse::rightClicked()) {
                                            *kb->value = 0;
                                            Mouse::consumeRight();
                                        }
                                    }
                                }
                                break;
                            }
                            }
                            drawlist->PopClipRect();
                            yOffset += visibleItemHeight;
                        }
                    }
                }
            }
            if (hasBody) {
                Vec4<float> bodyOutlineRect = snapRectToPixels(Vec4<float>(
                    headerRect.x, headerRect.w, headerRect.z, yOffset));
                ImU32 bodyOutline = IM_COL32(255, 255, 255, 10);
                drawlist->AddLine(ImVec2(bodyOutlineRect.x, bodyOutlineRect.y),
                    ImVec2(bodyOutlineRect.x, bodyOutlineRect.w - roundValue), bodyOutline, 1.0f);
                drawlist->AddLine(ImVec2(bodyOutlineRect.z, bodyOutlineRect.y),
                    ImVec2(bodyOutlineRect.z, bodyOutlineRect.w - roundValue), bodyOutline, 1.0f);
                drawlist->AddLine(ImVec2(bodyOutlineRect.x + roundValue, bodyOutlineRect.w),
                    ImVec2(bodyOutlineRect.z - roundValue, bodyOutlineRect.w), bodyOutline, 1.0f);
            }
        }

        lastKeyPress = -1;
    }
}


void ClickGUI::refreshConfigPanelEntries() {
	configPanelEntries.clear();
	const std::string configPath = FileUtil::getClientPath() + "Configs\\";
	if (!std::filesystem::exists(configPath)) return;

	for (const auto& entry : std::filesystem::directory_iterator(configPath)) {
		if (!entry.is_regular_file()) continue;
		if (entry.path().extension().string() != ".txt") continue;
		configPanelEntries.push_back(entry.path().stem().string());
	}

	std::sort(configPanelEntries.begin(), configPanelEntries.end());
}

void ClickGUI::renderBottomTabs(const Vec2<float>& screenSize, float alpha) {
	const Vec2<float>& mousePos = Mouse::pos();
	static Colors* colorsMod = ModuleManager::getModule<Colors>();
    ImGuiIO& io = ImGui::GetIO();
	ImDrawList* drawlist = ImGui::GetBackgroundDrawList();
    const float guiScale = std::clamp(scale, 0.8f, 1.6f);
	UIColor accentBase(colorsMod->getColor());
	UIColor accentColor(accentBase.r, accentBase.g, accentBase.b, (uint8_t)alphaByte(alpha, 120.f));

		const std::string names[] = {
		"GUI", "Config", "Settings"
	};
	const float dockPad = 3.f * guiScale;
	const float tabWidth = 90.f * guiScale;
	const float tabHeight = 24.f * guiScale;
	const float dockHeight = tabHeight + dockPad * 2.f;
	const float roundValue = 9.f * guiScale;
	const float tabsSlideY = (1.f - alpha) * 16.f * guiScale;
	const float searchGap = activeBottomPage == 0 ? 7.f * guiScale : 0.f;
	const float searchFocusTarget = (activeBottomPage == 0 && (isSearching || !searchingModule.empty())) ? 1.f : 0.f;
	searchAnim = Math::lerp(searchAnim, searchFocusTarget, std::clamp(io.DeltaTime * 12.f, 0.f, 1.f));
	float searchEase = easeOutCubic(searchAnim);
	float searchWidth = activeBottomPage == 0 ? (140.f + 34.f * searchEase) * guiScale : 0.f;
	const float maxDockWidth = std::max(270.f * guiScale, screenSize.x - 28.f * guiScale);
	const float baseDockWidth = dockPad * 2.f + tabWidth * 3.f + searchGap + searchWidth;
	if (baseDockWidth > maxDockWidth && activeBottomPage == 0) {
		searchWidth = std::max(92.f * guiScale, maxDockWidth - dockPad * 2.f - tabWidth * 3.f - searchGap);
	}

	const float dockWidth = dockPad * 2.f + tabWidth * 3.f + searchGap + searchWidth;
	const float dockX = (screenSize.x - dockWidth) * 0.5f;
	const float dockY = screenSize.y - dockHeight - 38.f * guiScale + tabsSlideY;
	Vec4<float> dockRect = snapRectToPixels(Vec4<float>(dockX, dockY, dockX + dockWidth, dockY + dockHeight));

	drawlist->AddRectFilled(ImVec2(dockRect.x + 2.f * guiScale, dockRect.y + 4.f * guiScale),
		ImVec2(dockRect.z + 2.f * guiScale, dockRect.w + 4.f * guiScale),
		IM_COL32(0, 0, 0, alphaByte(alpha, 55.f)), roundValue);
	drawEdgeGlow(drawlist, dockRect, roundValue, alpha, 8.f * guiScale);
	ImGuiRender::drawBlur(dockRect, roundValue, clickGuiBlurAlpha);
	drawlist->AddRectFilled(ImVec2(dockRect.x, dockRect.y),
		ImVec2(dockRect.z, dockRect.w), IM_COL32(10, 10, 12, alphaByte(alpha, 132.f)),
		roundValue);
	ImGuiRender::drawRectangle(dockRect, UIColor(255, 255, 255, alphaByte(alpha, 10.f)), 1.f, roundValue);

	const float tabY = dockRect.y + dockPad;
	const float startX = dockRect.x + dockPad;

	for (int i = 0; i < 3; i++) {
		Vec4<float> hitRect = Vec4<float>(startX + i * tabWidth, tabY, startX + (i + 1) * tabWidth, tabY + tabHeight);
		const bool active = activeBottomPage == i;
		const bool hovered = hitRect.contains(mousePos);
        tabAnim[i] = Math::lerp(tabAnim[i], (active || hovered) ? 1.f : 0.f,
            std::clamp(io.DeltaTime * 12.f, 0.f, 1.f));
        float tabEase = easeOutCubic(tabAnim[i]);

		Vec4<float> rect(hitRect.x + 2.f * guiScale, hitRect.y + 1.f * guiScale,
			hitRect.z - 2.f * guiScale, hitRect.w - 1.f * guiScale);
		const float itemRound = 6.f * guiScale;
		if (tabEase > 0.01f) {
			int fillAlpha = alphaByte(alpha, active ? (42.f + 40.f * tabEase) : 24.f * tabEase);
			drawEdgeGlow(drawlist, rect, itemRound, alpha * tabEase, 3.f * guiScale);
			drawlist->AddRectFilled(ImVec2(rect.x, rect.y), ImVec2(rect.z, rect.w),
				IM_COL32(18, 18, 20, fillAlpha), itemRound);
		}

		if (active) {
			drawlist->AddRect(ImVec2(rect.x, rect.y), ImVec2(rect.z, rect.w),
				accentColor.toImColor(alphaByte(alpha, 42.f)), itemRound, 0, 1.f);
			Vec4<float> activeLine(rect.x + 9.f * guiScale, rect.w - 2.f * guiScale,
				rect.z - 9.f * guiScale, rect.w - 1.f * guiScale);
			ImGuiRender::fillRectangle(activeLine, accentColor, 1.f * guiScale);
		}

		float labelSize = 0.66f * guiScale;
		int textAlpha = (int)((active || hovered ? 255.f : 178.f) * alpha);
		UIColor textColor(255, 255, 255, textAlpha);
		if (!active && !hovered)
			textColor = UIColor(172, 174, 182, textAlpha);
		Vec2<float> iconCenter(rect.x + 15.f * guiScale, rect.y + (rect.w - rect.y) * 0.5f);
		drawBottomTabIcon(drawlist, i, iconCenter, active ? accentColor : UIColor(172, 174, 182, textAlpha), guiScale * 0.86f);
		Vec2<float> textPos = Vec2<float>(rect.x + 27.f * guiScale,
			rect.y + ((rect.w - rect.y) - ImGuiRender::getTextHeight(labelSize)) * 0.5f - 1.f * guiScale);
		const float maxLabelWidth = rect.z - textPos.x - 5.f * guiScale;
		while (labelSize > 0.52f * guiScale && ImGuiRender::getTextWidth(names[i], labelSize) > maxLabelWidth)
			labelSize -= 0.02f * guiScale;
		textPos.y = rect.y + ((rect.w - rect.y) - ImGuiRender::getTextHeight(labelSize)) * 0.5f - 1.f * guiScale;
		const std::string label = fitTextToWidth(names[i], maxLabelWidth, labelSize);
		ImGuiRender::drawText(textPos, label, textColor, labelSize, active || hovered);

		if (hovered && Mouse::leftClicked()) {
            if (activeBottomPage != i)
                pageAnim = 0.f;
			activeBottomPage = i;
			configPanelScroll = 0;
			settingsPanelScroll = 0;
			for (auto& w : windowList) w->isDragging = false;
			draggingSliderSettingPtr = nullptr;
			capturingKbSettingPtr = nullptr;
			Mouse::consumeLeft();
		}
	}

	if (activeBottomPage == 0) {
		const float sbX = startX + tabWidth * 3.f + searchGap;
		Vec4<float> sbRect(sbX, tabY, sbX + searchWidth, tabY + tabHeight);
		const bool hovered = sbRect.contains(mousePos);

		if (hovered && Mouse::leftClicked()) {
			isSearching = true;
			Mouse::consumeLeft();
		}
		if (Mouse::leftClicked() && !hovered)
			isSearching = false;

		Vec4<float> inner(sbRect.x + 2.f * guiScale, sbRect.y + 2.f * guiScale,
			sbRect.z - 2.f * guiScale, sbRect.w - 2.f * guiScale);
		int sbAlpha = alphaByte(alpha, isSearching ? 82.f : (hovered ? 62.f : 38.f));
		drawEdgeGlow(drawlist, inner, 6.f * guiScale, alpha * (isSearching || hovered ? 0.9f : 0.45f), 4.f * guiScale);
		ImGuiRender::fillRectangle(inner, UIColor(18, 18, 20, sbAlpha), 6.f * guiScale);

		if (isSearching) {
			ImGuiRender::drawRectangle(inner, accentColor, 1.f, 6.f * guiScale);
		}
		else if (hovered) {
			ImGuiRender::drawRectangle(inner, UIColor(255, 255, 255, alphaByte(alpha, 16.f)), 1.f, 6.f * guiScale);
		}

		const float iconCX = inner.x + 12.f * guiScale;
		const float iconCY = inner.y + (inner.w - inner.y) * 0.5f;
		const float iconR = 4.2f * guiScale;
		ImColor iconCol = isSearching ? accentColor.toImColor(alphaByte(alpha, 210.f)) : UIColor(160, 162, 170, alphaByte(alpha, 185.f)).toImColor();
		ImGuiRender::renderCircle(Vec2<float>(iconCX, iconCY), iconCol, iconR, 1.2f);
		ImGuiRender::drawLine(Vec2<float>(iconCX + iconR * 0.7f, iconCY + iconR * 0.7f),
			Vec2<float>(iconCX + iconR * 1.7f, iconCY + iconR * 1.7f), isSearching ? accentColor : UIColor(160, 162, 170, alphaByte(alpha, 185.f)), 1.5f * guiScale);

		std::string display = searchingModule.empty() ? "Search..." : searchingModule;
		if (isSearching && !searchingModule.empty()) display += "_";
		UIColor textCol = searchingModule.empty() ? UIColor(135, 137, 145, alphaByte(alpha, 190.f)) : UIColor(235, 235, 238, alphaByte(alpha, 245.f));
		const float searchTextSize = 0.66f * guiScale;
		const float textX = inner.x + 24.f * guiScale;
		display = fitTextToWidth(display, inner.z - textX - 8.f * guiScale, searchTextSize);
		ImGuiRender::drawText(Vec2<float>(textX, inner.y + ((inner.w - inner.y) - ImGuiRender::getTextHeight(searchTextSize)) * 0.5f - 1.f * guiScale), display, textCol, searchTextSize);
	}
}

void ClickGUI::renderSettingsPanel(const Vec2<float>& screenSize, float alpha) {
	const Vec2<float>& mousePos = Mouse::pos();
	static Colors* colorsMod = ModuleManager::getModule<Colors>();
	UIColor accentColor(colorsMod->getColor());
	accentColor.a = (uint8_t)alphaByte(alpha, 135.f);
	const float guiScale = std::clamp(scale, 0.8f, 1.6f);
	ImDrawList* drawlist = ImGui::GetBackgroundDrawList();

	const float availableWidth = std::max(300.f, screenSize.x - 80.f * guiScale);
	const float availableHeight = std::max(230.f, screenSize.y - 165.f * guiScale);
	const float panelWidth = std::min(560.f * guiScale, availableWidth);
	const float panelHeight = std::min(430.f * guiScale, availableHeight);
	const float panelRound = 10.f * guiScale;
	float slideY = (1.f - alpha) * -12.f * guiScale;
	Vec4<float> panel = snapRectToPixels(Vec4<float>((screenSize.x - panelWidth) * 0.5f, 68.f * guiScale + slideY, (screenSize.x + panelWidth) * 0.5f, 68.f * guiScale + slideY + panelHeight));
	drawlist->AddRectFilled(ImVec2(panel.x + 2.f * guiScale, panel.y + 4.f * guiScale),
		ImVec2(panel.z + 2.f * guiScale, panel.w + 4.f * guiScale), IM_COL32(0, 0, 0, alphaByte(alpha, 55.f)), panelRound);
	drawEdgeGlow(drawlist, panel, panelRound, alpha, 10.f * guiScale);
	ImGuiRender::drawBlur(panel, panelRound, clickGuiBlurAlpha);
	drawlist->AddRectFilled(ImVec2(panel.x, panel.y), ImVec2(panel.z, panel.w),
		IM_COL32(10, 10, 12, alphaByte(alpha, 132.f)), panelRound);
	ImGuiRender::drawRectangle(panel, UIColor(255, 255, 255, alphaByte(alpha, 10.f)), 1.f, panelRound);

	const float titleSize = 0.92f * guiScale;
	const float subSize = 0.68f * guiScale;
	ImGuiRender::drawText(Vec2<float>(panel.x + 18.f * guiScale, panel.y + 14.f * guiScale),
		"Fonts", UIColor(255, 255, 255, alphaByte(alpha)), titleSize, true);

	const std::vector<FontManager::FontInfo> fonts = FontManager::getFonts();
	const std::string activeId = FontManager::getActiveFont();
	auto active = std::find_if(fonts.begin(), fonts.end(), [&](const auto& font) {
		return font.id == activeId;
	});
	std::string activeValue = std::string("Active: ") + (active != fonts.end() ? active->name : activeId);
	activeValue = fitTextToWidth(activeValue, panelWidth - 144.f * guiScale, subSize);
	ImGuiRender::drawText(Vec2<float>(panel.x + 18.f * guiScale, panel.y + 39.f * guiScale),
		activeValue, UIColor(170, 172, 180, alphaByte(alpha, 220.f)), subSize);

	const float listTop = panel.y + 68.f * guiScale;
	const float rowHeight = 38.f * guiScale;
	const float rowGap = 3.f * guiScale;
	const int maxRows = std::max(1, (int)((panel.w - listTop - 14.f * guiScale) / (rowHeight + rowGap)));

	{
		const int selectedFont = FontManager::getConfiguredFontIndex();
		const int maxScroll = std::max(0, (int)fonts.size() - maxRows);
		settingsPanelScroll = std::clamp(settingsPanelScroll, 0, maxScroll);
		const int rows = std::min((int)fonts.size() - settingsPanelScroll, maxRows);

		for (int i = 0; i < rows; ++i) {
			const int fontIndex = settingsPanelScroll + i;
			const FontManager::FontInfo& font = fonts[static_cast<std::size_t>(fontIndex)];
			const FontManager::FontState state = FontManager::getFontState(fontIndex);
			const float y = listTop + i * (rowHeight + rowGap);
			Vec4<float> row = snapListItemRectToPixels(Vec4<float>(panel.x + 12.f * guiScale, y, panel.z - 12.f * guiScale, y + rowHeight));
			const bool rowHovered = row.contains(mousePos);
			const bool selected = fontIndex == selectedFont;
			ImGuiRender::fillRectangle(row,
				selected ? UIColor(accentColor.r, accentColor.g, accentColor.b, alphaByte(alpha, 78.f))
					: UIColor(18, 18, 20, alphaByte(alpha, rowHovered ? 76.f : 42.f)),
				6.f * guiScale);
			if (selected)
				ImGuiRender::drawRectangle(row, accentColor, 1.f, 6.f * guiScale);

			const Vec2<float> radioCenter(row.x + 17.f * guiScale, (row.y + row.w) * 0.5f);
			ImGuiRender::renderCircle(radioCenter,
				(selected ? accentColor : UIColor(145, 147, 155, alphaByte(alpha, 210.f))).toImColor(),
				5.f * guiScale, 1.3f * guiScale);
			if (selected)
				ImGuiRender::renderCircleFilled(radioCenter, accentColor.toImColor(), 2.3f * guiScale);

			std::string status;
			UIColor statusColor(155, 157, 164, alphaByte(alpha, 230.f));
			switch (state) {
			case FontManager::FontState::BuiltIn:
				status = "Built-in";
				break;
			case FontManager::FontState::Ready:
				status = "Cached";
				statusColor = UIColor(105, 210, 130, alphaByte(alpha, 235.f));
				break;
			case FontManager::FontState::Downloading:
				status = "Downloading...";
				statusColor = accentColor;
				break;
			case FontManager::FontState::Failed:
				status = "Retry";
				statusColor = UIColor(225, 105, 105, alphaByte(alpha, 235.f));
				break;
			default:
				status = "Cloud";
				break;
			}

			const float statusSize = 0.6f * guiScale;
			const float statusWidth = ImGuiRender::getTextWidth(status, statusSize);
			const std::string name = fitTextToWidth(
				fontIndex == 0 ? "Default" : font.name,
				(row.z - row.x) - statusWidth - 66.f * guiScale, 0.74f * guiScale);
			ImGuiRender::drawText(Vec2<float>(row.x + 31.f * guiScale, row.y + 9.f * guiScale),
				name, UIColor(235, 235, 238, alphaByte(alpha)), 0.74f * guiScale, selected);
			ImGuiRender::drawText(Vec2<float>(row.z - statusWidth - 10.f * guiScale, row.y + 10.f * guiScale),
				status, statusColor, statusSize);

			if (rowHovered && Mouse::leftClicked() && state != FontManager::FontState::Downloading) {
				fontSelection = fontIndex;
				FontManager::setFontByIndex(fontIndex);
				if (fontSetting != nullptr)
					fontSetting->update();
				Mouse::consumeLeft();
			}
		}
	}
}

void ClickGUI::renderConfigPanel(const Vec2<float>& screenSize, float alpha) {
	const Vec2<float>& mousePos = Mouse::pos();
	static Colors* colorsMod = ModuleManager::getModule<Colors>();
	UIColor accentColor(colorsMod->getColor());
	accentColor.a = (uint8_t)alphaByte(alpha, 135.f);
	const float guiScale = std::clamp(scale, 0.8f, 1.6f);
	ImDrawList* drawlist = ImGui::GetBackgroundDrawList();

	refreshConfigPanelEntries();

	const float availableWidth = std::max(300.f, screenSize.x - 80.f * guiScale);
	const float availableHeight = std::max(230.f, screenSize.y - 165.f * guiScale);
	const float panelWidth = std::min(560.f * guiScale, availableWidth);
	const float panelHeight = std::min(430.f * guiScale, availableHeight);
	const float panelRound = 10.f * guiScale;
	float slideY = (1.f - alpha) * -12.f * guiScale;
	Vec4<float> panel = snapRectToPixels(Vec4<float>((screenSize.x - panelWidth) * 0.5f, 68.f * guiScale + slideY, (screenSize.x + panelWidth) * 0.5f, 68.f * guiScale + slideY + panelHeight));
	drawlist->AddRectFilled(ImVec2(panel.x + 2.f * guiScale, panel.y + 4.f * guiScale),
		ImVec2(panel.z + 2.f * guiScale, panel.w + 4.f * guiScale), IM_COL32(0, 0, 0, alphaByte(alpha, 55.f)), panelRound);
	drawEdgeGlow(drawlist, panel, panelRound, alpha, 10.f * guiScale);
	ImGuiRender::drawBlur(panel, panelRound, clickGuiBlurAlpha);
	drawlist->AddRectFilled(ImVec2(panel.x, panel.y), ImVec2(panel.z, panel.w),
		IM_COL32(10, 10, 12, alphaByte(alpha, 132.f)), panelRound);
	ImGuiRender::drawRectangle(panel, UIColor(255, 255, 255, alphaByte(alpha, 10.f)), 1.f, panelRound);

	std::string title = "Config Manager";
	std::string current = std::string("Current: ") + ConfigManager::currentConfig;
	const float titleSize = 0.92f * guiScale;
	const float subSize = 0.68f * guiScale;
	ImGuiRender::drawText(Vec2<float>(panel.x + 18.f * guiScale, panel.y + 14.f * guiScale), title, UIColor(255, 255, 255, alphaByte(alpha)), titleSize, true);
	ImGuiRender::drawText(Vec2<float>(panel.x + 18.f * guiScale, panel.y + 39.f * guiScale), current, UIColor(170, 172, 180, alphaByte(alpha, 220.f)), subSize);

	Vec4<float> createRect = Vec4<float>(panel.z - 200.f * guiScale, panel.y + 18.f * guiScale, panel.z - 114.f * guiScale, panel.y + 46.f * guiScale);
	const bool createHovered = createRect.contains(mousePos);
	const float headerBtnTextSize = 0.7f * guiScale;
	drawEdgeGlow(drawlist, createRect, 5.5f * guiScale, alpha * (createHovered ? 0.8f : 0.35f), 3.f * guiScale);
	ImGuiRender::fillRectangle(createRect, createHovered ? UIColor(accentColor.r, accentColor.g, accentColor.b, alphaByte(alpha, 96.f)) : UIColor(18, 18, 20, alphaByte(alpha, 58.f)), 5.5f * guiScale);
	const std::string createText = "Create";
	ImGuiRender::drawText(Vec2<float>(createRect.x + ((createRect.z - createRect.x) - ImGuiRender::getTextWidth(createText, headerBtnTextSize)) * 0.5f, createRect.y + 6.f * guiScale), createText, UIColor(255, 255, 255, alphaByte(alpha)), headerBtnTextSize);
	if (createHovered && Mouse::leftClicked()) {
		std::string newName = "new";
		for (int n = 2; ConfigManager::doesConfigExist(newName); n++)
			newName = "new-" + std::to_string(n);
		ConfigManager::createNewConfig(newName);
		refreshConfigPanelEntries();
		Mouse::consumeLeft();
	}

	Vec4<float> saveRect = Vec4<float>(panel.z - 108.f * guiScale, panel.y + 18.f * guiScale, panel.z - 18.f * guiScale, panel.y + 46.f * guiScale);
	const bool saveHovered = saveRect.contains(mousePos);
	drawEdgeGlow(drawlist, saveRect, 5.5f * guiScale, alpha * (saveHovered ? 0.8f : 0.35f), 3.f * guiScale);
	ImGuiRender::fillRectangle(saveRect, saveHovered ? UIColor(accentColor.r, accentColor.g, accentColor.b, alphaByte(alpha, 96.f)) : UIColor(18, 18, 20, alphaByte(alpha, 58.f)), 5.5f * guiScale);
	const std::string saveText = "Save";
	ImGuiRender::drawText(Vec2<float>(saveRect.x + ((saveRect.z - saveRect.x) - ImGuiRender::getTextWidth(saveText, headerBtnTextSize)) * 0.5f, saveRect.y + 6.f * guiScale), saveText, UIColor(255, 255, 255, alphaByte(alpha)), headerBtnTextSize);
	if (saveHovered && Mouse::leftClicked()) {
		if (ConfigManager::currentConfig != "NULL") {
			ConfigManager::saveConfig();
			Client::DisplayClientMessage("Successfully saved config %s%s%s!", MCTF::WHITE, ConfigManager::currentConfig.c_str(), MCTF::WHITE);
		}
		Mouse::consumeLeft();
	}

	const float listTop = panel.y + 68.f * guiScale;
	const float rowHeight = 34.f * guiScale;
	const float rowGap = 0.f;
	const float buttonWidth = 60.f * guiScale;
	const int maxRows = std::max(1, (int)((panel.w - listTop - 18.f * guiScale) / (rowHeight + rowGap)));
	const int maxScroll = std::max(0, (int)configPanelEntries.size() - maxRows);
	if (configPanelScroll > maxScroll) configPanelScroll = maxScroll;
	const int rows = std::min((int)configPanelEntries.size() - configPanelScroll, maxRows);

	for (int i = 0; i < rows; i++) {
		const std::string& configName = configPanelEntries[configPanelScroll + i];
		const float y = listTop + i * (rowHeight + rowGap);
		Vec4<float> row = snapListItemRectToPixels(Vec4<float>(panel.x + 12.f * guiScale, y, panel.z - 12.f * guiScale, y + rowHeight));
		const bool currentRow = configName == ConfigManager::currentConfig;
		const bool renamingThis = isRenaming && renameOldName == configName;
		const bool rowHovered = row.contains(mousePos);
		ImGuiRender::fillRectangle(row, currentRow ? UIColor(accentColor.r, accentColor.g, accentColor.b, alphaByte(alpha, 76.f)) : UIColor(18, 18, 20, alphaByte(alpha, rowHovered ? 76.f : 42.f)), 6.f * guiScale);

		const float rowTextSize = 0.76f * guiScale;
		std::string rowName = configName;
		rowName = fitTextToWidth(rowName, (row.z - row.x) - 220.f * guiScale, rowTextSize);

		if (renamingThis) {
			std::string displayText = renameBuffer + "_";
			displayText = fitTextToWidth(displayText, (row.z - row.x) - 220.f * guiScale, rowTextSize);
			ImGuiRender::drawText(Vec2<float>(row.x + 10.f * guiScale, row.y + 8.f * guiScale), displayText, UIColor(255, 255, 150, alphaByte(alpha)), rowTextSize);
		} else {
			ImGuiRender::drawText(Vec2<float>(row.x + 10.f * guiScale, row.y + 8.f * guiScale), rowName, UIColor(235, 235, 238, alphaByte(alpha)), rowTextSize);
		}

		Vec4<float> loadRect = Vec4<float>(row.z - buttonWidth * 3.f - 16.f * guiScale, row.y + 6.f * guiScale, row.z - buttonWidth * 2.f - 12.f * guiScale, row.w - 6.f * guiScale);
		Vec4<float> renameRect = Vec4<float>(row.z - buttonWidth * 2.f - 8.f * guiScale, row.y + 6.f * guiScale, row.z - buttonWidth - 4.f * guiScale, row.w - 6.f * guiScale);
		Vec4<float> saveRowRect = Vec4<float>(row.z - buttonWidth, row.y + 6.f * guiScale, row.z, row.w - 6.f * guiScale);
		const bool loadHovered = loadRect.contains(mousePos);
		const bool renameHovered = renameRect.contains(mousePos);
		const bool saveRowHovered = saveRowRect.contains(mousePos);

		auto drawBtn = [&](const Vec4<float>& r, bool hovered, const std::string& text, UIColor base, UIColor hover) {
			drawEdgeGlow(drawlist, r, 5.5f * guiScale, alpha * (hovered ? 0.8f : 0.3f), 3.f * guiScale);
			ImGuiRender::fillRectangle(r, hovered ? hover : base, 5.5f * guiScale);
			const float btnTextSize = 0.58f * guiScale;
			ImGuiRender::drawText(Vec2<float>(r.x + ((r.z - r.x) - ImGuiRender::getTextWidth(text, btnTextSize)) * 0.5f, r.y + 4.f * guiScale), text, UIColor(255, 255, 255, alphaByte(alpha)), btnTextSize);
		};

		drawBtn(loadRect, loadHovered, "Load",
			UIColor(18, 18, 20, alphaByte(alpha, 58.f)),
			UIColor(accentColor.r, accentColor.g, accentColor.b, alphaByte(alpha, 96.f)));
		drawBtn(renameRect, renameHovered, "Rename",
			UIColor(18, 18, 20, alphaByte(alpha, 58.f)),
			UIColor(accentColor.r, accentColor.g, accentColor.b, alphaByte(alpha, 96.f)));
		drawBtn(saveRowRect, saveRowHovered, saveText,
			UIColor(18, 18, 20, alphaByte(alpha, 58.f)),
			UIColor(accentColor.r, accentColor.g, accentColor.b, alphaByte(alpha, 96.f)));

		if (loadHovered && Mouse::leftClicked()) {
			isRenaming = false;
			ConfigManager::loadConfig(configName);
			Mouse::consumeLeft();
		}
		if (renameHovered && Mouse::leftClicked()) {
			isRenaming = true;
			renameOldName = configName;
			renameBuffer = configName;
			Mouse::consumeLeft();
		}
		if (saveRowHovered && Mouse::leftClicked()) {
			ConfigManager::currentConfig = configName;
			ConfigManager::saveConfig();
			Client::DisplayClientMessage("Successfully saved config %s%s%s!", MCTF::WHITE, ConfigManager::currentConfig.c_str(), MCTF::WHITE);
			Mouse::consumeLeft();
		}
	}

	if (configPanelEntries.empty()) {
		ImGuiRender::drawText(Vec2<float>(panel.x + 18.f * guiScale, listTop + 8.f * guiScale),
			"No configs found",
			UIColor(150, 152, 160, alphaByte(alpha)), 0.8f * guiScale);
	}
}

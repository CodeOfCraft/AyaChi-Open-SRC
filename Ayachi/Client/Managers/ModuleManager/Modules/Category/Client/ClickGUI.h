#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../../../../../Utils/ColorUtil.h"
#include "../../../../../../Utils/Maths.h"
#include "../../../../InputManager/MouseInput.h"
#include "../../ModuleBase/Module.h"

void getModuleListByCategory(Category category, std::vector<Module*>& modList);

struct ClickWindow {
    std::string windowName;
    Vec2<float> pos;
    float yOffset;
    bool extended = true;
    bool isDragging = false;
    Category category;
    std::vector<Module*> modList;
    float extendedAnim = 1.f;

    ClickWindow(const Vec2<float>& Pos, const Category& c) {
        this->pos = Pos;
        this->extended = true;
        if (c == Category::COMBAT)
            this->windowName = "Combat";
        else if (c == Category::MISC)
            this->windowName = "Misc";
        else if (c == Category::RENDER)
            this->windowName = "Render";
        else if (c == Category::MOVEMENT)
            this->windowName = "Movement";
        else if (c == Category::PLAYER)
            this->windowName = "Player";
        else if (c == Category::WORLD)
            this->windowName = "World";
        else if (c == Category::CLIENT)
            this->windowName = "Client";

        getModuleListByCategory(c, modList);
    }
};
struct Snowflake {
    float x, y;
    float size;
    float speed;
    float drift;
    float opacity;
    float phase;
};

class ClickGUI : public Module {
public:
    ClickGUI();
    ~ClickGUI();

    std::vector<ClickWindow*> windowList;
    Vec2<float> startDragPos = Vec2<float>(0.f, 0.f);
    float openDuration = 0.f;
    bool initClickGUI = false;
    void init();

    bool tooltips = false;
    bool isShiftDown = false;
    bool isBackKeyDown = false;
    std::string searchingModule;
    bool isSearching = false;
    int lastKeyPress = -1;
    bool* isChoosingKeyBindPtr = nullptr;

    float textSize = 1.f;
    float scale = 1.15f;
    float lastScale = 1.15f;
    SliderSetting<float>* scaleSetting = nullptr;
	int fontSelection = 0;
	FontSetting* fontSetting = nullptr;

    std::unordered_map<Module*, float> animProgress;
    std::unordered_map<Module*, float> hoverAnim;

    SliderSettingBase* draggingSliderSettingPtr = nullptr;
    KeybindSetting* capturingKbSettingPtr = nullptr;
    std::string textEditBuffer = "";

    int activeBottomPage = 0;
    float pageAnim = 1.f;
    float searchAnim = 0.f;
    float tabAnim[3] = {};
	int configPanelScroll = 0;
	int settingsPanelScroll = 0;
    bool isRenaming = false;
    std::string renameOldName;
    std::string renameBuffer;
    std::vector<std::string> configPanelEntries;

    void refreshConfigPanelEntries();
    void renderBottomTabs(const Vec2<float>& screenSize, float alpha);
    void renderConfigPanel(const Vec2<float>& screenSize, float alpha);
	void renderSettingsPanel(const Vec2<float>& screenSize, float alpha);

    void onKeyUpdate(int key, bool isDown) override;
    void render(ImDrawList* drawlist);
    virtual void onEnable() override;
    virtual void onDisable() override;

private:
    std::vector<Snowflake> snowflakes;
    bool snowInitialized = false;
    Vec2<float> snowViewport = Vec2<float>(0.f, 0.f);
    void initSnow(float width = 0.f, float height = 0.f);
    void renderBackground(ImDrawList* drawlist, const Vec2<float>& screenSize, UIColor accentColor, float alpha);
    void renderSnow(ImDrawList* drawlist, const Vec2<float>& screenSize, UIColor accentColor, float alpha, float yStart);
};

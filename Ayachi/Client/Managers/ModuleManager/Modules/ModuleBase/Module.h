#pragma once
#include <string>
#include <vector>
#include "../../../../../SDK/Runtime/GameContext.h"
#include "../../../../../SDK/Render/ItemInHandRenderer.h"
#include "../../../../../SDK/Render/TextMeasureData.h"
#include "../../../../../SDK/Render/CaretMeasureData.h"
#include "../../../../../SDK/World/Level/BlockSource.h"
#include "../../../../../Utils/TimerUtil.h"
#include "../../../../../Utils/StringUtil.h"
#include "../../../FriendManager/FriendManager.h"
#include "../../../../../Libs/json.hpp"

#include "Settings/BoolSetting.h"
#include "Settings/ColorSetting.h"
#include "Settings/EnumSetting.h"
#include "Settings/FontSetting.h"
#include "Settings/KeybindSetting.h"
#include "Settings/SliderSetting.h"
#include "Settings/Vec3Setting.h"

enum class Category {
	COMBAT = 0,
	MOVEMENT = 1,
	RENDER = 2,
	PLAYER = 3,
	WORLD = 4,
	MISC = 5,
	CLIENT = 6
};

class Packet;
class MinecraftUIRenderContext;
class LevelRenderer;
class ScreenContext;
class BaseActorRenderContext;
class Font;
class Matrix;
struct NinesliceInfo;
class Player;
class Block;
struct RectangleArea;
namespace mce {
	struct ClientTexture;
	struct Color;
}

class Module {
private:
	std::string name;
	std::string description;
	Category category;
	bool enabled = false;
	bool visible = true;
	int keybind = 0x0;
	int toggleMode = 0;

	std::vector<Setting*> settings;
public:
	float arraylistAnim = 0.f;
	float selectedAnim = 0.f;
	bool extended = false;
protected:
	inline Setting* registerSetting(Setting* setting) {
		this->settings.push_back(setting);
		return setting;
	}
public:
	Module(std::string moduleName, std::string des, Category c, int k = 0x0);
	virtual ~Module();

	inline std::string getModuleName() {
		return this->name;
	}

	inline std::string getDisplayName() const {
		return name;
	}

	inline std::string getDescription() const {
		return description;
	}

	inline Category getCategory() { 
		return this->category;
	}

	inline std::vector<Setting*>& getSettingList() {
		return this->settings;
	}

public:
	virtual std::string getModeText();
	virtual bool isEnabled();
	virtual bool isVisible();
	virtual bool isHoldMode();
	virtual int getKeybind();
	virtual void setKeybind(int key);
	virtual bool runOnBackground();
	virtual void setEnabled(bool enable);
	virtual void toggle();
	virtual void onDisable();
	virtual void onEnable();
	virtual void onKeyUpdate(int key, bool isDown);
	virtual void onClientTick();
	virtual void onTick(Level* level);
	virtual void onSendPacket(Packet* packet, bool& cancel);
	virtual void onReceivePacket(Packet* packet, bool& cancel);
	virtual void onSendChatMessage(std::string& message, bool& cancel);
	virtual void onDisplayClientMessage(std::string& message, bool& cancel);
	virtual void onGetGamma(float& gamma);
	virtual void onGetFov(float& fov);
	virtual void onGetPerspective(int& perspective);
	virtual void onGetHideHand(bool& hideHand);
	virtual void onLevelRender(LevelRenderer* levelRenderer, ScreenContext* screenContext);
	virtual void onRenderOutlineSelection(
		const Block& block,
		const BlockSource& blockSource,
		const BlockPos& position,
		bool& cancel);
	virtual void onRenderHitSelect(const BlockPos& position, bool& cancel);
	virtual void onRenderItemInHand(
		ItemInHandRenderer* renderer,
		BaseActorRenderContext& renderContext,
		const Matrix& previousProjection,
		ItemContextFlags itemFlags,
		bool& cancel);
	virtual void onRenderItemInOffhand(
		ItemInHandRenderer* renderer,
		BaseActorRenderContext& renderContext,
		Player& player,
		ItemContextFlags itemFlags,
		bool& cancel);
	virtual void onGetTimeOfDay(float& time);
	virtual void onMCRender(MinecraftUIRenderContext* renderCtx);
	virtual void onDrawText(
		MinecraftUIRenderContext* renderCtx,
		Font& font,
		const RectangleArea& area,
		std::string& text,
		const mce::Color& color,
		float alpha,
		ui::TextAlignment alignment,
		const TextMeasureData& textData,
		const CaretMeasureData& caretData,
		bool& cancel);
	virtual void onDrawImage(
		MinecraftUIRenderContext* renderCtx,
		const mce::ClientTexture& texture,
		const glm::vec2& position,
		const glm::vec2& size,
		const glm::vec2& uv,
		const glm::vec2& uvSize,
		bool colorCorrected,
		bool& cancel);
	virtual void onDrawNineslice(
		MinecraftUIRenderContext* renderCtx,
		const mce::ClientTexture& texture,
		const NinesliceInfo& info,
		bool& cancel);
	virtual void onLoadConfig(void* conf);
	virtual void onSaveConfig(void* conf);
	virtual void onImGuiRender(ImDrawList* drawlist);
};

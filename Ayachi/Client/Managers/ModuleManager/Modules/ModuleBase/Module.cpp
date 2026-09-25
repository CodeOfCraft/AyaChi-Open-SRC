#include "Module.h"
#include "../../../../../Libs/json.hpp"

#include "../../../NotificationManager/NotificationManager.h"

using json = nlohmann::json;

Module::Module(std::string moduleName, std::string des, Category c, int k) {
	this->name = moduleName;
	this->description = des;
	this->category = c;
	this->keybind = k;

	registerSetting(new BoolSetting("Visible", "Visible on arraylist", &visible, true));
	registerSetting(new KeybindSetting("Keybind", "Keybind of module", &keybind, k));
	registerSetting(new EnumSetting("Toggle", "How module should be toggled", { "Press", "Hold" }, &toggleMode, 0));
}

Module::~Module() {
	for (auto& setting : settings) {
		delete setting;
		setting = nullptr;
	}
	settings.clear();
}

std::string Module::getModeText() {
	return "NULL";
}

bool Module::isEnabled() {
	return enabled;
}

bool Module::isVisible() {
	return visible;
}

bool Module::isHoldMode() {
	return toggleMode;
}

int Module::getKeybind() {
	return keybind;
}

void Module::setKeybind(int key) {
	this->keybind = key;
}

bool Module::runOnBackground() {
	return false;
}

void Module::setEnabled(bool enable) {
	if (this->enabled != enable) {
		this->enabled = enable;

		if (enable) {
			this->onEnable();
		}
		else {
			this->onDisable();
		}

		extern void updateModuleCacheCallback();
		extern bool isInBatchUpdateMode();
		if (!isInBatchUpdateMode()) {
			updateModuleCacheCallback();

			std::string notiMsg = getDisplayName() + (enable ? " enabled" : " disabled");
			NotificationManager::addNotifBox(notiMsg, 2.f);
		}
	}
}

void Module::toggle() {
	setEnabled(!enabled);
}

void Module::onDisable() {
}

void Module::onEnable() {
}

void Module::onKeyUpdate(int key, bool isDown) {
	if (getKeybind() == key) {
		if (isHoldMode()) {
			setEnabled(isDown);
		}
		else {
			if (isDown) {
				toggle();
			}
		}
	}
}

void Module::onClientTick() {
}

void Module::onTick(Level*) {
}

void Module::onSendPacket(Packet*, bool&) {
}

void Module::onReceivePacket(Packet*, bool&) {
}

void Module::onSendChatMessage(std::string&, bool&) {
}

void Module::onDisplayClientMessage(std::string&, bool&) {
}

void Module::onGetGamma(float&) {
}

void Module::onGetFov(float&) {
}

void Module::onGetPerspective(int&) {
}

void Module::onGetHideHand(bool&) {
}

void Module::onLevelRender(LevelRenderer*, ScreenContext*) {
}

void Module::onRenderOutlineSelection(const Block&, const BlockSource&, const BlockPos&, bool&) {
}

void Module::onRenderHitSelect(const BlockPos&, bool&) {
}

void Module::onRenderItemInHand(
	ItemInHandRenderer*, BaseActorRenderContext&, const Matrix&, ItemContextFlags, bool&) {
}

void Module::onRenderItemInOffhand(
	ItemInHandRenderer*, BaseActorRenderContext&, Player&, ItemContextFlags, bool&) {
}

void Module::onGetTimeOfDay(float&) {
}

void Module::onMCRender(MinecraftUIRenderContext*) {
}

void Module::onDrawText(
	MinecraftUIRenderContext*, Font&, const RectangleArea&, std::string&, const mce::Color&,
	float, ui::TextAlignment, const TextMeasureData&, const CaretMeasureData&, bool&) {
}

void Module::onDrawImage(
	MinecraftUIRenderContext*, const mce::ClientTexture&, const glm::vec2&, const glm::vec2&,
	const glm::vec2&, const glm::vec2&, bool, bool&) {
}

void Module::onDrawNineslice(
	MinecraftUIRenderContext*, const mce::ClientTexture&, const NinesliceInfo&, bool&) {
}

void Module::onImGuiRender(ImDrawList* drawlist) {}

void Module::onLoadConfig(void* confVoid) {
	json* config = reinterpret_cast<json*>(confVoid);

	if (config == nullptr || !config->contains(getModuleName()))
		return;

	const json& obj = config->at(getModuleName());
	if (obj.is_null())
		return;

	if (obj.contains("enabled") && obj["enabled"].is_boolean())
		enabled = obj["enabled"].get<bool>();

	for (Setting* setting : this->settings) {
		try {
			setting->fromJson(obj);
		}
		catch (const std::exception& e) {
			Game::DisplayClientMessage("Config: failed to load setting %s in %s! %s", setting->name.c_str(), getModuleName().c_str(), e.what());
		}
	}
}

void Module::onSaveConfig(void* confVoid) {
	json* currentConfig = reinterpret_cast<json*>(confVoid);
	if (currentConfig == nullptr)
		return;

	json obj;
	obj["enabled"] = enabled;

	for (Setting* setting : this->settings) {
		try {
			setting->toJson(obj);
		}
		catch (const std::exception& e) {
			Game::DisplayClientMessage("Config: failed to save setting %s in %s! %s", setting->name.c_str(), getModuleName().c_str(), e.what());
		}
	}

	(*currentConfig)[getModuleName()] = obj;
}

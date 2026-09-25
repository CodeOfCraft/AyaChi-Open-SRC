#include "ModuleManager.h"
#include "../../Client.h"
#include "../../../Utils/HookGuard.h"
#include "../../../Utils/StringUtil.h"

#include "Modules/Category/Client/ClickGUI.h"
#include "Modules/Category/Client/HudEditor.h"
#include "Modules/Category/Client/Colors.h"
#include "Modules/Category/Client/Notification.h"
#include "Modules/Category/Misc/TestModule.h"
#include "Modules/Category/Movement/AutoSprint.h"
#include "Modules/Category/Movement/Fly.h"
#include "Modules/Category/Movement/Velocity.h"
#include "Modules/Category/Render/AutoScale.h"
#include "Modules/Category/Render/Arraylist.h"
#include "Modules/Category/Render/BlockHighlight.h"
#include "Modules/Category/Render/BreakIndicators.h"
#include "Modules/Category/Render/CustomFov.h"
#include "Modules/Category/Render/ESP.h"
#include "Modules/Category/Render/Fullbright.h"
#include "Modules/Category/Render/ItemPhysics.h"

void updateModuleCacheCallback() {
	ModuleManager::updateEnabledCache();
}

bool isInBatchUpdateMode() {
	return ModuleManager::batchUpdateMode;
}

void ModuleManager::updateEnabledCache() {
	try {
		auto enabled = std::make_shared<ModuleList>();
		auto background = std::make_shared<ModuleList>();

		for (auto* mod : moduleList) {
			if (mod->isEnabled()) {
				enabled->push_back(mod);
			}
			else if (mod->runOnBackground()) {
				background->push_back(mod);
			}
		}

		enabledSnapshot.store(std::move(enabled));
		backgroundSnapshot.store(std::move(background));
	} catch (...) {}
}

Module* ModuleManager::findModule(std::string_view name) {
	for (Module* module : moduleList) {
		if (module != nullptr &&
			(StringUtil::equalsIgnoreCase(name, module->getModuleName()) ||
			 StringUtil::equalsIgnoreCase(name, module->getDisplayName()))) {
			return module;
		}
	}
	return nullptr;
}

void ModuleManager::init() {
	moduleList.push_back(new ClickGUI());
	moduleList.push_back(new HudEditor());
	moduleList.push_back(new Colors());
	moduleList.push_back(new Notification());
	moduleList.push_back(new TestModule());
	moduleList.push_back(new AutoSprint());
	moduleList.push_back(new Fly());
	moduleList.push_back(new Velocity());
	moduleList.push_back(new AutoScale());
	moduleList.push_back(new Arraylist());
	moduleList.push_back(new BlockHighlight());
	moduleList.push_back(new BreakIndicators());
	moduleList.push_back(new CustomFov());
	moduleList.push_back(new ESP());
	moduleList.push_back(new Fullbright());
	moduleList.push_back(new ItemPhysics());

	std::sort(moduleList.begin(), moduleList.end(), [](Module* lhs, Module* rhs) {
		return lhs->getModuleName() < rhs->getModuleName();
	});

	getModule<Notification>()->setEnabled(true);

	allModulesSnapshot.store(std::make_shared<const ModuleList>(moduleList));

	updateEnabledCache();
}

void ModuleManager::shutdown() {
	try {
		batchUpdateMode = true;

		for (auto& mod : moduleList) {
			mod->setEnabled(false);
		}

		enabledSnapshot.store(std::make_shared<const ModuleList>());
		backgroundSnapshot.store(std::make_shared<const ModuleList>());
		allModulesSnapshot.store(std::make_shared<const ModuleList>());


		for (auto& mod : moduleList) {
			delete mod;
			mod = nullptr;
		}
		moduleList.clear();

		batchUpdateMode = false;
	} catch (...) { batchUpdateMode = false; }
}

void ModuleManager::onKeyUpdate(int key, bool isDown) {
	if (!Client::isInitialized())
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto all = allModulesSnapshot.load();
		for (auto* mod : *all) {
			mod->onKeyUpdate(key, isDown);
		}
	} catch (...) {}
}

void ModuleManager::onClientTick() {
	if (!Client::isInitialized())
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto enabled = enabledSnapshot.load();
		auto background = backgroundSnapshot.load();
		for (auto* mod : *enabled) {
			mod->onClientTick();
		}
		for (auto* mod : *background) {
			mod->onClientTick();
		}
	} catch (...) {}
}

void ModuleManager::onTick(Level* level) {
	if (!Client::isInitialized() || level == nullptr)
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto enabled = enabledSnapshot.load();
		auto background = backgroundSnapshot.load();
		for (auto* mod : *enabled) {
			mod->onTick(level);
		}
		for (auto* mod : *background) {
			mod->onTick(level);
		}
	} catch (...) {}
}

void ModuleManager::onSendPacket(Packet* packet, bool& cancel) {
	if (!Client::isInitialized() || packet == nullptr)
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto enabled = enabledSnapshot.load();
		auto background = backgroundSnapshot.load();
		for (auto* mod : *enabled)
			mod->onSendPacket(packet, cancel);
		for (auto* mod : *background)
			mod->onSendPacket(packet, cancel);
	} catch (...) {}
}

void ModuleManager::onReceivePacket(Packet* packet, bool& cancel) {
	if (!Client::isInitialized() || packet == nullptr)
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto enabled = enabledSnapshot.load();
		auto background = backgroundSnapshot.load();
		for (auto* mod : *enabled)
			mod->onReceivePacket(packet, cancel);
		for (auto* mod : *background)
			mod->onReceivePacket(packet, cancel);
	} catch (...) {}
}

void ModuleManager::dispatch(const std::function<void(Module*)>& callback) {
	if (!Client::isInitialized())
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto enabled = enabledSnapshot.load();
		auto background = backgroundSnapshot.load();
		for (auto* mod : *enabled)
			callback(mod);
		for (auto* mod : *background)
			callback(mod);
	} catch (...) {}
}

void ModuleManager::onSendChatMessage(std::string& message, bool& cancel) {
	dispatch([&](Module* mod) { mod->onSendChatMessage(message, cancel); });
}

void ModuleManager::onDisplayClientMessage(std::string& message, bool& cancel) {
	dispatch([&](Module* mod) { mod->onDisplayClientMessage(message, cancel); });
}

void ModuleManager::onGetGamma(float& gamma) {
	dispatch([&](Module* mod) { mod->onGetGamma(gamma); });
}

void ModuleManager::onGetFov(float& fov) {
	dispatch([&](Module* mod) { mod->onGetFov(fov); });
}

void ModuleManager::onGetPerspective(int& perspective) {
	dispatch([&](Module* mod) { mod->onGetPerspective(perspective); });
}

void ModuleManager::onGetHideHand(bool& hideHand) {
	dispatch([&](Module* mod) { mod->onGetHideHand(hideHand); });
}

void ModuleManager::onLevelRender(LevelRenderer* levelRenderer, ScreenContext* screenContext) {
	if (levelRenderer == nullptr || screenContext == nullptr)
		return;
	dispatch([&](Module* mod) { mod->onLevelRender(levelRenderer, screenContext); });
}

void ModuleManager::onRenderOutlineSelection(
	const Block& block,
	const BlockSource& blockSource,
	const BlockPos& position,
	bool& cancel) {
	dispatch([&](Module* mod) { mod->onRenderOutlineSelection(block, blockSource, position, cancel); });
}

void ModuleManager::onRenderHitSelect(const BlockPos& position, bool& cancel) {
	dispatch([&](Module* mod) { mod->onRenderHitSelect(position, cancel); });
}

void ModuleManager::onRenderItemInHand(
	ItemInHandRenderer* renderer,
	BaseActorRenderContext& renderContext,
	const Matrix& previousProjection,
	ItemContextFlags itemFlags,
	bool& cancel) {
	dispatch([&](Module* mod) {
		mod->onRenderItemInHand(renderer, renderContext, previousProjection, itemFlags, cancel);
	});
}

void ModuleManager::onRenderItemInOffhand(
	ItemInHandRenderer* renderer,
	BaseActorRenderContext& renderContext,
	Player& player,
	ItemContextFlags itemFlags,
	bool& cancel) {
	dispatch([&](Module* mod) {
		mod->onRenderItemInOffhand(renderer, renderContext, player, itemFlags, cancel);
	});
}

void ModuleManager::onGetTimeOfDay(float& time) {
	dispatch([&](Module* mod) { mod->onGetTimeOfDay(time); });
}

void ModuleManager::onMCRender(MinecraftUIRenderContext* renderCtx) {
	if (!Client::isInitialized() || renderCtx == nullptr)
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto enabled = enabledSnapshot.load();
		auto background = backgroundSnapshot.load();
		for (auto* mod : *enabled)
			mod->onMCRender(renderCtx);
		for (auto* mod : *background)
			mod->onMCRender(renderCtx);
	} catch (...) {}
}

void ModuleManager::onDrawText(
	MinecraftUIRenderContext* renderCtx,
	Font& font,
	const RectangleArea& area,
	std::string& text,
	const mce::Color& color,
	float alpha,
	ui::TextAlignment alignment,
	const TextMeasureData& textData,
	const CaretMeasureData& caretData,
	bool& cancel) {
	dispatch([&](Module* mod) {
		mod->onDrawText(
			renderCtx, font, area, text, color, alpha, alignment, textData, caretData, cancel);
	});
}

void ModuleManager::onDrawImage(
	MinecraftUIRenderContext* renderCtx,
	const mce::ClientTexture& texture,
	const glm::vec2& position,
	const glm::vec2& size,
	const glm::vec2& uv,
	const glm::vec2& uvSize,
	bool colorCorrected,
	bool& cancel) {
	dispatch([&](Module* mod) {
		mod->onDrawImage(
			renderCtx, texture, position, size, uv, uvSize, colorCorrected, cancel);
	});
}

void ModuleManager::onDrawNineslice(
	MinecraftUIRenderContext* renderCtx,
	const mce::ClientTexture& texture,
	const NinesliceInfo& info,
	bool& cancel) {
	dispatch([&](Module* mod) { mod->onDrawNineslice(renderCtx, texture, info, cancel); });
}


void ModuleManager::onLoadConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto all = allModulesSnapshot.load();
		for (auto* mod : *all) {
			try {
				mod->onLoadConfig(conf);
			} catch (...) {}
		}

		updateEnabledCache();
	} catch (...) {}
}

void ModuleManager::onSaveConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto all = allModulesSnapshot.load();
		for (auto* mod : *all) {
			try {
				mod->onSaveConfig(conf);
			} catch (...) {}
		}
	} catch (...) {}
}

void ModuleManager::onImGuiRender(ImDrawList* drawlist) {
	if (!Client::isInitialized())
		return;

	HookGuard::Scope guard;
	if (!guard)
		return;

	try {
		auto enabled = enabledSnapshot.load();
		auto background = backgroundSnapshot.load();
		for (auto* mod : *enabled) {
			mod->onImGuiRender(drawlist);
		}
		for (auto* mod : *background) {
			mod->onImGuiRender(drawlist);
		}
	} catch (...) {}
}

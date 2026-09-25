#pragma once
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../ModuleManager/Modules/Category/Client/ClickGUI.h"
#include "../../../ModuleManager/Modules/Category/Client/HudEditor.h"
#include "../../../../Client.h"
#include "../../../../../SDK/Runtime/GameContext.h"
#include "../../../../../SDK/Input/Keyboard.h"

namespace InputPoller {
	inline void poll() {
		if (!Client::isInitialized())
			return;

		if (Addresses::KeyPressFunc != 0 || Game::keyMapPtr == nullptr)
			return;

		static ClickGUI* clickGuiMod = ModuleManager::getModule<ClickGUI>();
		static HudEditor* hudEditorMod = ModuleManager::getModule<HudEditor>();
		static Keyboard keyboard(reinterpret_cast<int*>(Game::keyMapPtr));

		keyboard.poll([](int key, bool isDown) {
			if (hudEditorMod != nullptr && hudEditorMod->isEnabled()) {
				hudEditorMod->onKeyUpdate(key, isDown);
				return;
			}
			if (clickGuiMod != nullptr) {
				if (!clickGuiMod->isEnabled() && key == clickGuiMod->getKeybind()) {
					clickGuiMod->onKeyUpdate(key, isDown);
					return;
				}
				if (clickGuiMod->isEnabled()) {
					clickGuiMod->onKeyUpdate(key, isDown);
					return;
				}
			}
			if (Game::canUseMoveKeys())
				ModuleManager::onKeyUpdate(key, isDown);
		});
	}
}

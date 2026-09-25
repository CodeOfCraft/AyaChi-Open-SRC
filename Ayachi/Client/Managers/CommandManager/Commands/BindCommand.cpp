#include "BindCommand.h"
#include "../../../Client.h"
#include "../../../../Utils/StringUtil.h"

#include <iterator>

BindCommand::BindCommand() : CommandBase("bind", "Binds modules to specific keys", "<module> <key>", { "b" }) {
}

bool BindCommand::execute(const std::vector<std::string>& args) {
	if (args.size() < 3)
		return false;

	const std::string& moduleNeedToFind = args[1];
	Module* currentModule = ModuleManager::findModule(moduleNeedToFind);

	if (currentModule == nullptr) {
		Client::DisplayClientMessage("Couldn't find module with name: %s%s", MCTF::GRAY, moduleNeedToFind.c_str());
		return true;
	}

	const std::string& key = args[2];
	if (key.size() > 1) {
		std::string normalizedKey = StringUtil::toLower(key);
		if (normalizedKey == "none") {
			currentModule->setKeybind(0x0);
			Client::DisplayClientMessage("Successfully unbound %s%s", MCTF::GRAY, currentModule->getModuleName().c_str());
			return true;
		}
		if (normalizedKey.starts_with("vk_"))
			normalizedKey.erase(0, 3);
		if (normalizedKey == "enter") normalizedKey = "return";
		else if (normalizedKey == "esc") normalizedKey = "escape";
		else if (normalizedKey == "spacebar") normalizedKey = "space";
		else if (normalizedKey == "pageup") normalizedKey = "prior";
		else if (normalizedKey == "pagedown") normalizedKey = "next";

		for (std::size_t i = 1; i < std::size(KeyNames); ++i) {
			const char* haystack = KeyNames[i];
			if (haystack == nullptr || StringUtil::equalsIgnoreCase(haystack, "Unknown"))
				continue;

			std::string keyName = StringUtil::toLower(haystack);
			if (keyName.starts_with("vk_"))
				keyName.erase(0, 3);

			if (normalizedKey == keyName) {
				currentModule->setKeybind(static_cast<int>(i));
				Client::DisplayClientMessage("The keybind of %s%s%s is now '%s%s%s'", MCTF::GRAY, currentModule->getModuleName().c_str(), MCTF::RESET, MCTF::GRAY, haystack, MCTF::RESET);
				return true;
			}
		}
		Client::DisplayClientMessage("%sInvalid key!", MCTF::RED);
		return true;
	}
	const SHORT mappedKey = VkKeyScanA(key[0]);
	const int keyCode = mappedKey == -1 ? 0 : LOBYTE(mappedKey);
	if (keyCode > 0 && keyCode < static_cast<int>(std::size(KeyNames))) {
		currentModule->setKeybind(keyCode);
		Client::DisplayClientMessage("The keybind of %s%s%s is now '%s%c%s'", MCTF::GRAY, currentModule->getModuleName().c_str(), MCTF::RESET, MCTF::GRAY, (char)keyCode, MCTF::RESET);
		return true;
	}
	else {
		Client::DisplayClientMessage("%sInvalid key!", MCTF::RED);
		return true;
	}
}

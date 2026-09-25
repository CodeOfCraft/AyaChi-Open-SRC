#include "ToggleCommand.h"
#include "../../ModuleManager/ModuleManager.h"
#include "../../../Client.h"

ToggleCommand::ToggleCommand() : CommandBase("toggle", "Toggles a module on/off", "<module>", { "t" }) {
}

bool ToggleCommand::execute(const std::vector<std::string>& args) {
	if (args.size() < 2)
		return false;

	const std::string& moduleNeedToFind = args[1];
	Module* currentModule = ModuleManager::findModule(moduleNeedToFind);

	if (currentModule == nullptr) {
		Client::DisplayClientMessage("Couldn't find module with name: %s%s", MCTF::GRAY, moduleNeedToFind.c_str());
		return true;
	}

	bool newState = !currentModule->isEnabled();
	currentModule->setEnabled(newState);
	Client::DisplayClientMessage("%s%s %s%s", MCTF::GRAY, currentModule->getModuleName().c_str(), newState ? MCTF::GREEN : MCTF::RED, newState ? "enabled" : "disabled");
	return true;
}

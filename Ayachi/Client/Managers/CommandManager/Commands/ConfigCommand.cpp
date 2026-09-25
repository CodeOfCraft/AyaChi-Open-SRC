#include "ConfigCommand.h"
#include "../../../Client.h"
#include "../../../../Utils/StringUtil.h"

ConfigCommand::ConfigCommand() : CommandBase("config", "Create/load/save configs", "<load/new/save> <name>", { "c" }) {
}

bool ConfigCommand::execute(const std::vector<std::string>& args) {
	if (args.size() < 2)
		return false;

	const std::string action = StringUtil::toLower(args[1]);
	if (action == "load") {
		if (args.size() < 3)
			return false;
		const std::string name = StringUtil::join(args, 2);
		ConfigManager::loadConfig(name);
		return true;
	}
	else if (action == "new") {
		if (args.size() < 3)
			return false;
		const std::string name = StringUtil::join(args, 2);
		ConfigManager::createNewConfig(name);
		return true;
	}
	else if (action == "save") {
		if (ConfigManager::currentConfig == "NULL") {
			Client::DisplayClientMessage("%sNo active config to save.", MCTF::RED);
			return true;
		}
		ConfigManager::saveConfig();
		Client::DisplayClientMessage("Successfully saved config %s%s%s!", MCTF::GRAY, ConfigManager::currentConfig.c_str(), MCTF::WHITE);
		return true;
	}
	return false;
}

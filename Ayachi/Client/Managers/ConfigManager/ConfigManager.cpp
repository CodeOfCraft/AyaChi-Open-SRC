#include "ConfigManager.h"
#include "../../Client.h"
#include "../../../Utils/FileUtil.h"

#include "../ModuleManager/ModuleManager.h"
#include "../NotificationManager/NotificationManager.h"

void ConfigManager::init() {
	configsPath = FileUtil::getClientPath() + "Configs\\";
	if (!FileUtil::doesFilePathExist(configsPath))
		FileUtil::createPath(configsPath);
}

bool ConfigManager::doesConfigExist(const std::string& name) {
	std::string path = configsPath + name + configFormat;
	return FileUtil::doesFilePathExist(path);
}

void ConfigManager::createNewConfig(const std::string& name) {
	if (doesConfigExist(name)) {
		Client::DisplayClientMessage("%sFailed to create config %s%s%s. Config already existed!", MCTF::RED, MCTF::GRAY, name.c_str(), MCTF::RED);
		return;
	}

	if (currentConfig != "NULL") {
		saveConfig();
	}

	currentConfig = name;
	saveConfig();

	Client::DisplayClientMessage("Successfully created config %s%s%s!", MCTF::GRAY, name.c_str(), MCTF::WHITE);
}

void ConfigManager::loadConfig(const std::string& name) {
	if (!doesConfigExist(name)) {
		Client::DisplayClientMessage("%sFailed to load config %s%s%s. Config doesn't exist!", MCTF::RED, MCTF::GRAY, name.c_str(), MCTF::RED);
		return;
	}

	currentConfig = name;

	std::string configPath = configsPath + name + configFormat;

	std::ifstream confFile(configPath.c_str(), std::ifstream::binary);
	currentConfigObj.clear();
	bool parsedOk = true;
	try {
		confFile >> currentConfigObj;
	}
	catch (const std::exception& e) {
		parsedOk = false;
		currentConfigObj = json::object();

		confFile.close();
		std::error_code ec;
		std::filesystem::copy_file(configPath, configPath + ".corrupt",
			std::filesystem::copy_options::overwrite_existing, ec);

		Client::DisplayClientMessage("%sConfig %s%s%s is corrupt, loading defaults (backup: %s.corrupt)! %s%.120s",
			MCTF::RED, MCTF::GRAY, name.c_str(), MCTF::RED, name.c_str(), MCTF::GRAY, e.what());
	}
	ModuleManager::onLoadConfig((void*)&currentConfigObj);
	if (currentConfigObj.contains("friends")) {
		FriendManager::setFriendList(currentConfigObj["friends"]);
	}
	if (currentConfigObj.contains("name")) Client::setClientName(currentConfigObj["name"]);
	if (parsedOk)
		Client::DisplayClientMessage("Successfully loaded config %s%s%s!", MCTF::GRAY, name.c_str(), MCTF::WHITE);
}

void ConfigManager::saveConfig() {
	if (currentConfig == "NULL")
		return;

	std::string configPath = configsPath + currentConfig + configFormat;

	ModuleManager::onSaveConfig((void*)&currentConfigObj);
	currentConfigObj["friends"] = FriendManager::getList();
	currentConfigObj["name"] = Client::getClientName();
	std::ofstream o(configPath, std::ifstream::binary);
	o << std::setw(4) << currentConfigObj << std::endl;
	o.flush();
	o.close();
}

void ConfigManager::renameConfig(const std::string& oldName, const std::string& newName) {
	if (!doesConfigExist(oldName)) {
		Client::DisplayClientMessage("%sFailed to rename config %s%s%s. Config doesn't exist!", MCTF::RED, MCTF::GRAY, oldName.c_str(), MCTF::RED);
		return;
	}
	if (doesConfigExist(newName)) {
		Client::DisplayClientMessage("%sFailed to rename config to %s%s%s. Name already exists!", MCTF::RED, MCTF::GRAY, newName.c_str(), MCTF::RED);
		return;
	}

	std::string oldPath = configsPath + oldName + configFormat;
	std::string newPath = configsPath + newName + configFormat;

	std::filesystem::rename(oldPath, newPath);

	if (currentConfig == oldName)
		currentConfig = newName;

	Client::DisplayClientMessage("Successfully renamed config %s%s %sto%s %s%s!", MCTF::GRAY, oldName.c_str(), MCTF::WHITE, MCTF::WHITE, MCTF::GRAY, newName.c_str());
}

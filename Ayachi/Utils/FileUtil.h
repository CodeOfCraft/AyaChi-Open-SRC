#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace FileUtil {

	std::string getRoamingStatePath();
	std::string getClientPath();

	inline bool doesFilePathExist(const std::string& path) {
		return std::filesystem::exists(path);
	}

	inline bool createPath(const std::string& path) {
		std::error_code ec;
		if (std::filesystem::create_directories(path, ec))
			return true;
		return !ec && std::filesystem::is_directory(path, ec);
	}

	inline bool deletePath(const std::string& path) {
		return std::filesystem::remove(path);
	}
}

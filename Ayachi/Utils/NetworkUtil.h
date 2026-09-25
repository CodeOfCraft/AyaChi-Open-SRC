#pragma once
#include "../Libs/xorstr.hpp"
#include <string>

namespace NetworkUtil {
	bool canAccessInternet();
	bool downloadFile(const std::string& name, const std::string& path, const std::string& url);
	bool sendWebhook(const char* botName, const char* content, const char* webhookAddr);
	std::string getStrings(const std::string& url);
}

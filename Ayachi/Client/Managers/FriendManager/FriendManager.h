#pragma once
#include <string>
#include <vector>

class FriendManager {
public:
	static std::vector<std::string> getList();
	static bool isInList(const std::string& name);
	static bool addToList(const std::string& name);
	static bool removeFromList(const std::string& name);
	static void setFriendList(std::vector<std::string> newList);
};

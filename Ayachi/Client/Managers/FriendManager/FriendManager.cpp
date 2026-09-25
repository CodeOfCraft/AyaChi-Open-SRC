#include "FriendManager.h"
#include <algorithm>
#include <mutex>

namespace {
	std::vector<std::string> friendList;
	std::mutex friendListMutex;
}

std::vector<std::string> FriendManager::getList() {
	std::lock_guard<std::mutex> lock(friendListMutex);
	return friendList;
}

bool FriendManager::isInList(const std::string& name) {
	std::lock_guard<std::mutex> lock(friendListMutex);
	return std::find(friendList.begin(), friendList.end(), name) != friendList.end();
}

bool FriendManager::addToList(const std::string& name) {
	std::lock_guard<std::mutex> lock(friendListMutex);
	if (std::find(friendList.begin(), friendList.end(), name) != friendList.end())
		return false;

	friendList.push_back(name);
	return true;
}

bool FriendManager::removeFromList(const std::string& name) {
	std::lock_guard<std::mutex> lock(friendListMutex);
	const auto it = std::find(friendList.begin(), friendList.end(), name);
	if (it == friendList.end())
		return false;

	friendList.erase(it);
	return true;
}

void FriendManager::setFriendList(std::vector<std::string> newList) {
	std::lock_guard<std::mutex> lock(friendListMutex);
	friendList = std::move(newList);
}

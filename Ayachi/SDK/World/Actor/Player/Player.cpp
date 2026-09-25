#include "Player.h"

void Player::displayClientMessage(
	const std::string& message,
	const std::optional<std::string>& filteredMessage) {
	MemoryUtil::CallVFunc<
		GameData::VTableIndex::Player::displayClientMessage,
		void,
		const std::string&,
		const std::optional<std::string>&>(this, message, filteredMessage);
}

std::string Player::getXUID() {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Player::getXUID, std::string>(this);
}

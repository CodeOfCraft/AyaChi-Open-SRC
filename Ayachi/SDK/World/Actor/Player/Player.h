#pragma once
#include <optional>
#include <string>
#include "../Mob.h"
#include "../../GameMode/GameMode.h"

class PlayerInventory;

class Player : public Mob {
public:
	void displayClientMessage(
		const std::string& message,
		const std::optional<std::string>& filteredMessage = std::nullopt);
	std::string getXUID();

	CLASS_MEMBER(PlayerInventory*, supplies, 0x500);
	CLASS_MEMBER(GameMode*, gameMode, 0x9E8);
	CLASS_MEMBER(std::string, playerName, 0xB08);
};

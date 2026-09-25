#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

struct GuiMessage {
	int type;
	std::string message;
	std::optional<std::string> filteredMessage;
	std::string ttsMessage;
	std::string username;
	std::string fullString;
	std::optional<std::string> filteredFullString;
	std::string authorXuid;
	bool forceVisible;
	bool ttsRequired;
	float duration;
	bool hasBeenSeen;
	bool hasEverBeenSeen;
	int direction;
	bool isLocalPlayer;
};

static_assert(offsetof(GuiMessage, message) == 0x8);
static_assert(offsetof(GuiMessage, filteredMessage) == 0x28);
static_assert(offsetof(GuiMessage, filteredFullString) == 0xB0);
static_assert(sizeof(GuiMessage) == 0x110);

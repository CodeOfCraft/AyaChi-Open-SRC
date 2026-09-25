#pragma once

#include <array>
#include <cstdint>
#include <string>

enum class CommandOriginType : uint8_t {
	Player = 0,
	CommandBlock = 1,
	MinecartCommandBlock = 2,
	DevConsole = 3,
	Test = 4,
	AutomationPlayer = 5,
	ClientAutomation = 6,
	DedicatedServer = 7,
	Entity = 8,
	Virtual = 9,
	GameArgument = 10,
	EntityServer = 11,
	Precompiled = 12,
	GameDirectorEntityServer = 13,
	Scripting = 14,
	ExecuteContext = 15,
};

enum class CurrentCmdVersion : int32_t {
	Invalid = -1,
	Initial = 1,
	Latest = 50,
};

struct CommandOriginData {
	CommandOriginType mType;
	std::array<uint8_t, 7> mTypePadding{};
	std::array<uint64_t, 2> mUuid{};
	std::string mRequestId;
	int64_t mPlayerId = 0;
};

static_assert(sizeof(CommandOriginData) == 0x40);

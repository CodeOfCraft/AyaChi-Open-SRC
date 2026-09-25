#pragma once

#include <cstdint>

enum class Difficulty : int32_t {
	Peaceful = 0,
	Easy = 1,
	Normal = 2,
	Hard = 3,
	Count = 4,
	Unknown = 5,
};

enum class GameType : int32_t {
	Undefined = -1,
	Survival = 0,
	Creative = 1,
	Adventure = 2,
	Default = 5,
	Spectator = 6,
	WorldDefault = 0,
};

#pragma once

#include <cstdint>

enum class ActorSwingSource : uint8_t {
	None = 0,
	Build = 1,
	Mine = 2,
	Interact = 3,
	Attack = 4,
	UseItem = 5,
	ThrowItem = 6,
	DropItem = 7,
	Event = 8,
};

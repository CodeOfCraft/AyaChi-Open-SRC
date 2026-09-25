#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct WasOnGroundFlagComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x85C63FFD;
};

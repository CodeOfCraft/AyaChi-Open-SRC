#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct VerticalCollisionFlagComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xC6A02A9A;
};

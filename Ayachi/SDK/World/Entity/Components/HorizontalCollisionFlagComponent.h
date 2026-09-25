#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct HorizontalCollisionFlagComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xD5EE0BD8;

	bool collidedOnXAxis;
	bool collidedOnZAxis;
};

static_assert(sizeof(HorizontalCollisionFlagComponent) == 0x2);

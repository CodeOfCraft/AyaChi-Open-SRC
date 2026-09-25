#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct ApplyGravityComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x48C5D5FE;

	float gravity;
	bool resetFallDistance;
};

static_assert(sizeof(ApplyGravityComponent) == 0x8);

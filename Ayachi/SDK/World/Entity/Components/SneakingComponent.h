#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct SneakingComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x8918332C;

	float sneakingMovementFactor;
};

static_assert(sizeof(SneakingComponent) == 0x4);

#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct FallDistanceComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xCE6B34F6;

	float value;
	bool ignoresFallDamage;
	bool spawnParticlesWhenHittingGround;
};

static_assert(sizeof(FallDistanceComponent) == 0x8);

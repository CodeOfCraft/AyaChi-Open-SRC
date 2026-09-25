#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct OnFireComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x7E613FF3;

	int32_t onFireTicks;
	bool ignitedThisTick;
};

static_assert(sizeof(OnFireComponent) == 0x8);

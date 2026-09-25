#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct MovementSpeedComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x1268662E;

	float value;
};

static_assert(sizeof(MovementSpeedComponent) == 0x4);

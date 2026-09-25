#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct MobBodyRotationComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xD7F64BBA;

	float bodyYaw;
	float previousBodyYaw;
};

static_assert(sizeof(MobBodyRotationComponent) == 0x8);

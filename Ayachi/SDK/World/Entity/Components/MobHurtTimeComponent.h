#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct MobHurtTimeComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xD7A3585C;

	int32_t value;
};

static_assert(sizeof(MobHurtTimeComponent) == 0x4);

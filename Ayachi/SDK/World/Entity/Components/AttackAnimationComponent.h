#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct AttackAnimationComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xB5659F20;

	int32_t attackAnimationTick;
};

static_assert(sizeof(AttackAnimationComponent) == 0x4);

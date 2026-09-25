#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct JumpTicksComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xAC923948;

	int32_t value;
};

static_assert(sizeof(JumpTicksComponent) == 0x4);

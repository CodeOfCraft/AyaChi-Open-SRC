#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct IsDeadFlagComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x710A62C4;
};

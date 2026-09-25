#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct FreezeImmuneFlagComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x251A6DC0;
};

#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct ActorTypeComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x4F6BA419;

	uint32_t type;
};

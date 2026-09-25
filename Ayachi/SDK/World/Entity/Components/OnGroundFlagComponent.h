#pragma once

#include <cstdint>

#include "../IEntityComponent.h"

struct OnGroundFlagComponent : IEntityComponent {
	static constexpr std::uint32_t type_hash = 0xC29078A0;
};

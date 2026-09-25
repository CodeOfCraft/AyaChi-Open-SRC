#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct ActorDiedComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x51E0E2EF;
};

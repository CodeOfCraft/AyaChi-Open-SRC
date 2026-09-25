#pragma once
#include <cstdint>
#include "../../Actor/ActorId.h"
#include "../IEntityComponent.h"

struct ActorUniqueIDComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x18F957AF;

	ActorUniqueID uniqueID;
};

static_assert(sizeof(ActorUniqueIDComponent) == 0x8);

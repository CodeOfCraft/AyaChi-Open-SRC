#pragma once
#include <cstdint>
#include "../../../Network/Packet/GameTypes.h"
#include "../IEntityComponent.h"

struct ActorGameTypeComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x88D3EDDF;

	GameType gameType;
};

static_assert(sizeof(ActorGameTypeComponent) == 0x4);

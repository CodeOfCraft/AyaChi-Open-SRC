#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct ActorHeadRotationComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xBABE7211;

	float headYaw;
	float previousHeadYaw;
};

static_assert(sizeof(ActorHeadRotationComponent) == 0x8);

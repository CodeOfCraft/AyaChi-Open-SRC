#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct ActorWalkAnimationComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x666777C6;

	float walkAnimSpeedMultiplier;
	float walkAnimSpeedOld;
	float walkAnimSpeed;
	float walkAnimPos;
	float movementThisTick;
};

static_assert(sizeof(ActorWalkAnimationComponent) == 0x14);

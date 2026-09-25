#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct AgeableComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x9A06FF03;

	int32_t age;
	bool growthPaused;
};

static_assert(sizeof(AgeableComponent) == 0x8);

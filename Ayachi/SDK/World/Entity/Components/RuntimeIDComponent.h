#pragma once
#include <cstdint>
#include "../IEntityComponent.h"

struct RuntimeIDComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xFC0DBBB5;

	uint64_t runtimeID;
};

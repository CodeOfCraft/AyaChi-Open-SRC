#pragma once

#include <cstddef>

struct alignas(8) ActorDimensionRef {
	std::byte storage[0x10];
};

static_assert(sizeof(ActorDimensionRef) == 0x10);

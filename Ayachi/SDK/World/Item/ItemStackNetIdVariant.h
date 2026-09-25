#pragma once

#include <cstddef>

struct alignas(8) ItemStackNetIdVariant {
	std::byte mStorage[0x18];
};

static_assert(sizeof(ItemStackNetIdVariant) == 0x18);
static_assert(alignof(ItemStackNetIdVariant) == 8);

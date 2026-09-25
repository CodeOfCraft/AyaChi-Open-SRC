#pragma once

#include <type_traits>

class InteractionResult {
public:
	bool mSuccess : 1;
	bool mSwing : 1;
};

static_assert(sizeof(InteractionResult) == 1);
static_assert(alignof(InteractionResult) == 1);
static_assert(std::is_trivially_copyable_v<InteractionResult>);

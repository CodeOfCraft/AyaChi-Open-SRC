#pragma once

#include <cstdint>
#include <deque>
#include <optional>

#include "Matrix.h"

class MatrixStack {
public:
	std::deque<Matrix> stack;
	std::optional<std::uint64_t> sortOrigin;
	bool isDirty;
};

static_assert(sizeof(std::deque<Matrix>) == 0x28);
static_assert(sizeof(MatrixStack) == 0x40);

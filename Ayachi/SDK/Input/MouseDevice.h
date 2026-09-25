#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include "MouseAction.h"

class MouseDevice {
public:
	int32_t index;
	int16_t x;
	int16_t y;
	int16_t dx;
	int16_t dy;
	int16_t xOld;
	int16_t yOld;
	char buttonStates[5];
	std::vector<MouseAction> inputs;
	int32_t firstMovementType;

	static MouseDevice* get();
};

static_assert(offsetof(MouseDevice, inputs) == 0x18);
static_assert(offsetof(MouseDevice, firstMovementType) == 0x30);
static_assert(sizeof(MouseDevice) == 0x38);

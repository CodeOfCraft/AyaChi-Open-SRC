#pragma once
#include <cstdint>
#include "../../../../Utils/Maths.h"
#include "../IEntityComponent.h"

struct RenderRotationComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xD15944E2;

	Vec2<float> rotation;
};

static_assert(sizeof(RenderRotationComponent) == 0x8);

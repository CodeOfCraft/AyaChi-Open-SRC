#pragma once
#include <cstdint>
#include "../../../../Utils/Maths.h"
#include "../IEntityComponent.h"

struct AABBShapeComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0xBAC1B3CF;

	AABB boundingBox;
	Vec2<float> size;
};

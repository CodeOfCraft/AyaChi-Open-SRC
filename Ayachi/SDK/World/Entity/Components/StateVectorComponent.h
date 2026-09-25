#pragma once
#include <cstdint>
#include "../../../../Utils/Maths.h"
#include "../IEntityComponent.h"

struct StateVectorComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x1B5D5238;

	Vec3<float> pos;
	Vec3<float> posOld;
	Vec3<float> velocity;
};

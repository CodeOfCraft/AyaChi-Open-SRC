#pragma once
#include <cstdint>
#include "../../../../Utils/Maths.h"
#include "../IEntityComponent.h"

struct ActorRotationComponent : IEntityComponent {
	static constexpr uint32_t type_hash = 0x75DF36B7;

	Vec2<float> rotation;
	Vec2<float> rotationOld;
};

#pragma once

#include <cstdint>

#include "../../World/Actor/ActorId.h"
#include "../../World/Actor/ActorSwingSource.h"

enum class AnimateAction : uint8_t {
	NoAction = 0,
	Swing = 1,
	WakeUp = 3,
	CriticalHit = 4,
	MagicCriticalHit = 5,
};

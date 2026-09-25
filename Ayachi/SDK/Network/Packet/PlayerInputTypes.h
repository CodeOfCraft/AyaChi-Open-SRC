#pragma once

#include <cstdint>

enum class InputMode : uint32_t {
	Undefined = 0,
	Mouse = 1,
	Touch = 2,
	GamePad = 3,
	MotionControllerDeprecated = 4,
	Count = 5,
};

enum class ClientPlayMode : uint32_t {
	Normal = 0,
	Teaser = 1,
	Screen = 2,
	ViewerDeprecated = 3,
	RealityDeprecated = 4,
	PlacementDeprecated = 5,
	LivingRoomDeprecated = 6,
	ExitLevel = 7,
	ExitLevelLivingRoomDeprecated = 8,
	NumModes = 9,
};

enum class NewInteractionModel : int32_t {
	Touch = 0,
	Crosshair = 1,
	Classic = 2,
	Count = 3,
};

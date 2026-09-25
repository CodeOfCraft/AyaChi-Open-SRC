#pragma once

#include "../../../Utils/MemoryUtil.h"

class Level;

class GameSession {
public:
	CLASS_MEMBER(Level**, level, 0x40);

	Level* getLevel();
};

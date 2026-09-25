#pragma once
#include "../../../Utils/MemoryUtil.h"

class Level;
class Timer;
class GameSession;

class Minecraft {
public:
	Level* getLevel();

	CLASS_MEMBER(GameSession*, gameSession, 0xC0);
	CLASS_MEMBER(Timer*, timer, 0xD8);
};

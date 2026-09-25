#pragma once
#include "../../Utils/MemoryUtil.h"
#include "LevelRendererPlayer.h"

class LevelRenderer {
	CLASS_MEMBER(LevelRendererPlayer*, levelRendererPlayer, 0x468);
public:
	LevelRendererPlayer* getLevelRendererPlayer() {
		return levelRendererPlayer;
	}
};

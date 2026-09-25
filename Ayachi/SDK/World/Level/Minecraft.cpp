#include "Minecraft.h"
#include "GameSession.h"

Level* Minecraft::getLevel() {
	GameSession* session = gameSession;
	return session ? session->getLevel() : nullptr;
}

Level* GameSession::getLevel() {
	return reinterpret_cast<Level*>(level);
}

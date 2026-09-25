#pragma once
#include <string>
#include <vector>
#include "../../../Utils/MemoryUtil.h"
#include "../Actor/ActorId.h"

class Actor;
class PacketSender;
class Player;

class Level {
public:
	Actor* fetchEntity(ActorUniqueID actorId, bool getRemoved = false) const;
	Actor* getRuntimeEntity(ActorRuntimeID actorId, bool getRemoved = false) const;
	Player* getPlayer(const std::string& name) const;
	Player* getPlayer(ActorUniqueID actorId) const;
	Player* getPlayerByXuid(const std::string& xuid) const;
	Player* getPlatformPlayer(const std::string& platformOnlineId) const;
	Player* getPlayerFromServerId(const std::string& serverId) const;
	Player* getRuntimePlayer(ActorRuntimeID runtimeId) const;
	int getNumRemotePlayers() const;
	Player* getPrimaryLocalPlayer() const;

	void tick();
	int getTime() const;
	void setTime(int time);
	uint32_t getSeed();

	bool isClientSide() const;
	std::vector<Actor*> getRuntimeActorList() const;
	PacketSender* getPacketSender() const;
	bool hasStartWithMapEnabled() const;
	bool isEditorWorld() const;
	bool getAllowAnonymousBlockDropsInEditorWorlds() const;
	bool isHardcore() const;
	bool getTearingDown() const;

	CLASS_MEMBER(std::string, name, 0x290);
};

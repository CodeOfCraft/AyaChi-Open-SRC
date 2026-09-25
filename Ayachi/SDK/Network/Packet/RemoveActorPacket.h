#pragma once

#include "Packet.h"
#include "../../World/Actor/ActorId.h"

class RemoveActorPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::RemoveActor;

	ActorUniqueID mEntityId;
	int32_t mSerializationMode;
};

static_assert(sizeof(RemoveActorPacket) == 0x38);

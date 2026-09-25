#pragma once

#include "Packet.h"
#include "GameTypes.h"

class SetPlayerGameTypePacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::SetPlayerGameType;

	GameType mPlayerGameType;
	int32_t mSerializationMode;
};

static_assert(sizeof(SetPlayerGameTypePacket) == 0x30);

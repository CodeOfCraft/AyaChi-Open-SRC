#pragma once

#include "Packet.h"

class SetHealthPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::SetHealth;

	int32_t mHealth;
	int32_t mSerializationMode;
};

static_assert(sizeof(SetHealthPacket) == 0x30);

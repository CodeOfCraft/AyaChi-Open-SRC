#pragma once

#include "Packet.h"

class SetTimePacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::SetTime;

	int32_t mTime;
	int32_t mSerializationMode;
};

static_assert(sizeof(SetTimePacket) == 0x30);

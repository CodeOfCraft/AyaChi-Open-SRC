#pragma once

#include "Packet.h"

class RequestChunkRadiusPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::RequestChunkRadius;

	int32_t mChunkRadius;
	uint8_t mMaxChunkRadius;
	int32_t mSerializationMode;
};

static_assert(sizeof(RequestChunkRadiusPacket) == 0x38);

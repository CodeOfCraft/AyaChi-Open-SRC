#pragma once

#include "Packet.h"

class ChunkRadiusUpdatedPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::ChunkRadiusUpdated;

	int32_t mChunkRadius;
	int32_t mSerializationMode;
};

static_assert(sizeof(ChunkRadiusUpdatedPacket) == 0x30);

#pragma once

#include <string>

#include "Packet.h"
#include "../../World/Actor/ActorId.h"

class EmotePacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::Emote;

	ActorRuntimeID mRuntimeId;
	std::string mPieceId;
	uint32_t mEmoteTicks;
	std::string mXuid;
	std::string mPlatformId;
	uint8_t mFlags;
	int32_t mSerializationMode;
};

static_assert(sizeof(EmotePacket) == 0xA0);

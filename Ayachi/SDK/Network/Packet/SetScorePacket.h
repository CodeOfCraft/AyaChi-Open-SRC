#pragma once

#include <vector>

#include "Packet.h"
#include "ScorePacketInfo.h"

class SetScorePacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::SetScore;

	std::vector<ScorePacketInfo> mScoreInfo;
	int32_t mSerializationMode;
};

static_assert(sizeof(SetScorePacket) == 0x48);

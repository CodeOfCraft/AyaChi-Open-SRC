#pragma once

#include "Packet.h"
#include "GameTypes.h"

class SetDifficultyPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::SetDifficulty;

	Difficulty mDifficulty;
	int32_t mSerializationMode;
};

static_assert(sizeof(SetDifficultyPacket) == 0x30);

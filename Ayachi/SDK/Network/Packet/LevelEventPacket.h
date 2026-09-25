#pragma once

#include "Packet.h"
#include "../../../Utils/Maths.h"

class LevelEventPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::LevelEvent;

	int32_t mEventId;
	Vec3<float> mPos;
	int32_t mData;
	int32_t mSerializationMode;
};

static_assert(sizeof(LevelEventPacket) == 0x40);

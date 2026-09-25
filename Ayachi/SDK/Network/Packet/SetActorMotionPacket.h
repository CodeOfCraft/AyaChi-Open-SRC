#pragma once

#include "Packet.h"
#include "../../World/Actor/ActorId.h"
#include "../../../Utils/Maths.h"

class SetActorMotionPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::SetActorMotion;

	ActorRuntimeID mRuntimeId;
	Vec3<float> mMotion;
	uint64_t mTick;
	int32_t mSerializationMode;
};

static_assert(sizeof(SetActorMotionPacket) == 0x50);

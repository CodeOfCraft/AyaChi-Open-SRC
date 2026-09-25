#pragma once

#include "Packet.h"
#include "InteractionTypes.h"
#include "../../World/Actor/ActorId.h"
#include "../../../Utils/Maths.h"

class PlayerActionPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::PlayerAction;

	BlockPos mPos;
	BlockPos mResultPos;
	int32_t mFace;
	PlayerActionType mAction;
	ActorRuntimeID mRuntimeId;
	bool mIsFromServerPlayerMovementSystem;
	int32_t mSerializationMode;
};

static_assert(sizeof(PlayerActionPacket) == 0x58);

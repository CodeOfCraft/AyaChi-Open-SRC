#pragma once

#include <optional>

#include "Packet.h"
#include "InteractionTypes.h"
#include "../../World/Actor/ActorId.h"
#include "../../../Utils/Maths.h"

struct MovePlayerTeleportData {
	int32_t mCause;
	int32_t mSourceEntityType;
};

class MovePlayerPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::MovePlayer;

	ActorRuntimeID mPlayerID;
	Vec3<float> mPos;
	Vec2<float> mRot;
	float mYHeadRot;
	PlayerPositionMode mResetPosition;
	bool mOnGround;
	ActorRuntimeID mRidingID;
	std::optional<MovePlayerTeleportData> mTeleportData;
	uint64_t mTick;
	int32_t mSerializationMode;
};

static_assert(sizeof(MovePlayerTeleportData) == 0x8);
static_assert(sizeof(std::optional<MovePlayerTeleportData>) == 0xC);
static_assert(sizeof(MovePlayerPacket) == 0x78);

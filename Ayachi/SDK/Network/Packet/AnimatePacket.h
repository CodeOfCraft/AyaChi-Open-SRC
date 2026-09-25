#pragma once

#include <optional>

#include "Packet.h"
#include "ActorPacketTypes.h"

class AnimatePacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::Animate;

	ActorRuntimeID mRuntimeId;
	AnimateAction mAction;
	float mData;
	std::optional<ActorSwingSource> mSwingSource;
	int32_t mSerializationMode;
};

static_assert(sizeof(std::optional<ActorSwingSource>) == 0x2);
static_assert(sizeof(AnimatePacket) == 0x40);

#pragma once

#include "Packet.h"
#include "ContainerTypes.h"

class PlayerHotbarPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::PlayerHotbar;

	uint32_t mSelectedSlot;
	bool mShouldSelectSlot;
	ContainerID mContainerId;
	int32_t mSerializationMode;
};

static_assert(sizeof(PlayerHotbarPacket) == 0x38);

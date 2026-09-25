#pragma once

#include "Packet.h"
#include "ContainerTypes.h"

class ContainerClosePacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::ContainerClose;

	ContainerID mContainerId;
	ContainerType mContainerType;
	bool mServerInitiatedClose;
	int32_t mSerializationMode;
};

static_assert(sizeof(ContainerClosePacket) == 0x30);

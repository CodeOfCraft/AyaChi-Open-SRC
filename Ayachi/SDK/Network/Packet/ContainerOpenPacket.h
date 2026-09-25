#pragma once

#include "Packet.h"
#include "ContainerTypes.h"
#include "../../World/Actor/ActorId.h"
#include "../../../Utils/Maths.h"

class ContainerOpenPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::ContainerOpen;

	ContainerID mContainerId;
	ContainerType mType;
	BlockPos mPos;
	ActorUniqueID mEntityUniqueID;
	int32_t mSerializationMode;
};

static_assert(sizeof(ContainerOpenPacket) == 0x48);

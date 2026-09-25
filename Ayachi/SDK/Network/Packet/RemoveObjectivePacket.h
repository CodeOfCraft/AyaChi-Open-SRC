#pragma once

#include <string>

#include "Packet.h"

class RemoveObjectivePacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::RemoveObjective;

	std::string mObjectiveName;
	int32_t mSerializationMode;
};

static_assert(sizeof(RemoveObjectivePacket) == 0x50);

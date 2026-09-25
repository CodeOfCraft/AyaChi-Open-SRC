#pragma once

#include <array>
#include <string>

#include "Packet.h"
#include "CommandTypes.h"

class CommandRequestPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::CommandRequest;

	std::string mCommand;
	CommandOriginData mOrigin;
	CurrentCmdVersion mVersion;
	bool mInternalSource;
	std::array<std::byte, 3> mPayloadPadding;
	int32_t mSerializationMode;
};

static_assert(sizeof(CommandRequestPacket) == 0x98);

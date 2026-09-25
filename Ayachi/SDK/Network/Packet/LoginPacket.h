#pragma once

#include "Packet.h"

class LoginPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::Login;

	int32_t mClientNetworkVersion;
	void* mConnectionRequest;
	int32_t mSerializationMode;
};

static_assert(sizeof(LoginPacket) == 0x40);

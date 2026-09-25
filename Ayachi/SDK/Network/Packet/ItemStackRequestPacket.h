#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "Packet.h"

class ItemStackRequestPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::ItemStackRequest;

	std::vector<std::byte> mRequests;
	int32_t mSerializationMode;
};

static_assert(sizeof(ItemStackRequestPacket) == 0x48);

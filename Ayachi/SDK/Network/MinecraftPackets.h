#pragma once

#include <memory>

#include "Packet/Packet.h"
#include "../Runtime/GameData.h"

class MinecraftPackets {
public:
	static std::shared_ptr<Packet> createPacket(PacketID id) {
		if (Addresses::MinecraftPackets_createPacket == 0)
			return {};

		using func_t = std::shared_ptr<Packet>(__cdecl*)(PacketID);
		return reinterpret_cast<func_t>(Addresses::MinecraftPackets_createPacket)(id);
	}
};

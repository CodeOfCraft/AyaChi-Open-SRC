#pragma once

#include "../Runtime/GameData.h"
#include "../../Utils/MemoryUtil.h"

class Packet;

class PacketSender {
public:
	bool isInitialized() const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::PacketSender::isInitialized, bool>(
			const_cast<PacketSender*>(this));
	}

	void send(Packet& packet) {
		MemoryUtil::CallVFunc<GameData::VTableIndex::PacketSender::send, void, Packet&>(
			this, packet);
	}

	void sendToServer(Packet& packet) {
		MemoryUtil::CallVFunc<GameData::VTableIndex::PacketSender::sendToServer, void, Packet&>(
			this, packet);
	}
};

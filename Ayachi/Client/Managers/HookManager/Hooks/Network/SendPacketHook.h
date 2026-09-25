#pragma once

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Network/Packet/Packet.h"
#include "../../../../../SDK/Network/PacketSender.h"
#include "../../../../../Utils/HookGuard.h"

class SendPacketHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(PacketSender*, Packet&);
	static inline func_t oFunc;

	static void sendToServerCallback(PacketSender* sender, Packet& packet) {
		HookGuard::Scope guard;
		bool cancel = false;
		if (guard)
			ModuleManager::onSendPacket(&packet, cancel);

		if (!cancel)
			oFunc(sender, packet);
	}

public:
	SendPacketHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&sendToServerCallback);
	}
};

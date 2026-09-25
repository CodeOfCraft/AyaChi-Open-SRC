#pragma once

#include <memory>

#include "../FuncHook.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../../../SDK/Network/Packet/Packet.h"
#include "../../../../../Utils/HookGuard.h"

class NetEventCallback;
class NetworkIdentifier;

template <PacketID Id>
class ReceivePacketHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		void*, const NetworkIdentifier&, NetEventCallback&, std::shared_ptr<Packet>&);
	static inline func_t oFunc;

	static void callback(
		void* dispatcher,
		const NetworkIdentifier& source,
		NetEventCallback& netEventCallback,
		std::shared_ptr<Packet>& packet) {
		HookGuard::Scope guard;
		bool cancel = false;
		if (guard && packet)
			ModuleManager::onReceivePacket(packet.get(), cancel);

		if (!cancel)
			oFunc(dispatcher, source, netEventCallback, packet);
	}

public:
	ReceivePacketHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&callback);
	}
};

using TextPacketHook = ReceivePacketHook<PacketID::Text>;
using LevelEventPacketHook = ReceivePacketHook<PacketID::LevelEvent>;
using LevelSoundEventPacketHook = ReceivePacketHook<PacketID::LevelSoundEvent>;
using ActorEventPacketHook = ReceivePacketHook<PacketID::ActorEvent>;
using RemoveActorPacketHook = ReceivePacketHook<PacketID::RemoveActor>;
using SetActorMotionPacketHook = ReceivePacketHook<PacketID::SetActorMotion>;
using SetScorePacketHook = ReceivePacketHook<PacketID::SetScore>;

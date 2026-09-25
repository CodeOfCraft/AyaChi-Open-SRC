#include "HookManager.h"

#include <memory>
#include <string>

#include "Hooks/Network/ReceivePacketHook.h"
#include "Hooks/Network/SendPacketHook.h"
#include "../../../SDK/Runtime/GameContext.h"
#include "../../../SDK/Network/MinecraftPackets.h"
#include "../../../SDK/Network/PacketSender.h"
#include "../../../SDK/Runtime/GameData.h"
#include "../../../Utils/Logger.h"

void HookManager::registerNetworkHooks() {
	std::string missingPacketHandlers;
	{
		PacketSender* packetSender = Game::clientInstance->getPacketSender();
		if (packetSender != nullptr) {
			auto** packetSenderVTable = *reinterpret_cast<uintptr_t***>(packetSender);
			RequestHook<SendPacketHook>(
				packetSenderVTable, GameData::VTableIndex::PacketSender::sendToServer);
		}
		else {
			LOGW("PacketSender is unavailable; outbound packet hooks are disabled");
		}

		auto getPacketHandlerVTable = [&missingPacketHandlers](PacketID id, const char* name) -> uintptr_t** {
			std::shared_ptr<Packet> packet = MinecraftPackets::createPacket(id);
			if (!packet) {
				if (!missingPacketHandlers.empty())
					missingPacketHandlers += ", ";
				missingPacketHandlers += name;
				return nullptr;
			}

			void* handler = packet->getHandler();
			if (handler != nullptr)
				return *reinterpret_cast<uintptr_t***>(handler);

			if (!missingPacketHandlers.empty())
				missingPacketHandlers += ", ";
			missingPacketHandlers += name;
			return nullptr;
		};

		constexpr int handleIndex = GameData::VTableIndex::PacketHandlerDispatcher::handle;
		RequestHook<TextPacketHook>(getPacketHandlerVTable(PacketID::Text, "Text"), handleIndex);
		RequestHook<LevelEventPacketHook>(
			getPacketHandlerVTable(PacketID::LevelEvent, "LevelEvent"), handleIndex);
		RequestHook<LevelSoundEventPacketHook>(
			getPacketHandlerVTable(PacketID::LevelSoundEvent, "LevelSoundEvent"), handleIndex);
		RequestHook<ActorEventPacketHook>(
			getPacketHandlerVTable(PacketID::ActorEvent, "ActorEvent"), handleIndex);
		RequestHook<RemoveActorPacketHook>(
			getPacketHandlerVTable(PacketID::RemoveActor, "RemoveActor"), handleIndex);
		RequestHook<SetActorMotionPacketHook>(
			getPacketHandlerVTable(PacketID::SetActorMotion, "SetActorMotion"), handleIndex);
		RequestHook<SetScorePacketHook>(
			getPacketHandlerVTable(PacketID::SetScore, "SetScore"), handleIndex);
	}
	if (!missingPacketHandlers.empty())
		LOGW("Inbound packet handlers unavailable: %s", missingPacketHandlers.c_str());
}

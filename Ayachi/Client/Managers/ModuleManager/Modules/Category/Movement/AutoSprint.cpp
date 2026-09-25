#include "AutoSprint.h"

#include "../../../../../../SDK/Network/Packet/PlayerAuthInputPacket.h"
#include "../../../../../../SDK/World/Actor/Player/LocalPlayer.h"

AutoSprint::AutoSprint()
	: Module("AutoSprint", "Automatically sprint while moving", Category::MOVEMENT) {
}

bool AutoSprint::runOnBackground() {
	return cleanupPending.load(std::memory_order_acquire);
}

void AutoSprint::onDisable() {
	sprintRequested.store(false, std::memory_order_release);
	packetSprinting.store(false, std::memory_order_release);
	cleanupPending.store(true, std::memory_order_release);
}

void AutoSprint::onTick(Level*) {
	LocalPlayer* localPlayer = Game::getLocalPlayer();

	if (cleanupPending.exchange(false, std::memory_order_acq_rel)) {
		if (localPlayer != nullptr)
			localPlayer->setSprinting(false);

		extern void updateModuleCacheCallback();
		updateModuleCacheCallback();
		return;
	}

	if (localPlayer != nullptr)
		localPlayer->setSprinting(sprintRequested.load(std::memory_order_acquire));
}

void AutoSprint::onSendPacket(Packet* packet, bool&) {
	if (!isEnabled() || packet == nullptr)
		return;

	PlayerAuthInputPacket* inputPacket = packet->as<PlayerAuthInputPacket>();
	if (inputPacket == nullptr)
		return;

	constexpr float movementEpsilon = 0.0001f;
	const float movementLengthSquared =
		inputPacket->mMove.x * inputPacket->mMove.x +
		inputPacket->mMove.y * inputPacket->mMove.y;
	const bool shouldSprint =
		movementLengthSquared > movementEpsilon &&
		!inputPacket->hasInputData(InputData::Sneaking) &&
		!inputPacket->hasInputData(InputData::SneakDown);
	const bool wasSprinting =
		packetSprinting.exchange(shouldSprint, std::memory_order_acq_rel);

	inputPacket->setInputData(InputData::SprintDown, shouldSprint);
	inputPacket->setInputData(InputData::Sprinting, shouldSprint);
	inputPacket->setInputData(InputData::StartSprinting, shouldSprint && !wasSprinting);
	inputPacket->setInputData(InputData::StopSprinting, !shouldSprint && wasSprinting);

	sprintRequested.store(shouldSprint, std::memory_order_release);
}

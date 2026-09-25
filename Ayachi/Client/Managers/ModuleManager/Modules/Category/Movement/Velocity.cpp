#include "Velocity.h"

#include "../../../../../../SDK/Network/Packet/SetActorMotionPacket.h"
#include "../../../../../../SDK/World/Actor/Player/LocalPlayer.h"

Velocity::Velocity()
	: Module("Velocity", "Scale incoming knockback", Category::MOVEMENT) {
	registerSetting(new SliderSetting<float>(
		"Horizontal", "Horizontal knockback percentage", &horizontal, horizontal, 0.f, 100.f));
	registerSetting(new SliderSetting<float>(
		"Vertical", "Vertical knockback percentage", &vertical, vertical, 0.f, 100.f));
}

std::string Velocity::getModeText() {
	char text[24]{};
	sprintf_s(text, "H%.0f%% V%.0f%%", horizontal, vertical);
	return text;
}

void Velocity::onReceivePacket(Packet* packet, bool&) {
	if (packet == nullptr)
		return;

	SetActorMotionPacket* motionPacket = packet->as<SetActorMotionPacket>();
	if (motionPacket == nullptr)
		return;

	LocalPlayer* localPlayer = Game::getLocalPlayer();
	if (localPlayer == nullptr || motionPacket->mRuntimeId.rawID != localPlayer->getRuntimeID())
		return;

	const float horizontalMultiplier = horizontal / 100.f;
	const float verticalMultiplier = vertical / 100.f;
	motionPacket->mMotion.x *= horizontalMultiplier;
	motionPacket->mMotion.y *= verticalMultiplier;
	motionPacket->mMotion.z *= horizontalMultiplier;
}

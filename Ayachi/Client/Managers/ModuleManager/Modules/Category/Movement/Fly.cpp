#include "Fly.h"

#include <Windows.h>
#include <cmath>

#include "../../../../../../SDK/Runtime/GameContext.h"
#include "../../../../../../SDK/Input/Keyboard.h"
#include "../../../../../../SDK/Network/Packet/PlayerAuthInputPacket.h"
#include "../../../../../../SDK/World/Actor/Player/LocalPlayer.h"

Fly::Fly()
	: Module("Fly", "Fly using directional motion", Category::MOVEMENT) {
	registerSetting(new SliderSetting<float>(
		"Horizontal Speed", "Horizontal flight speed", &horizontalSpeed,
		horizontalSpeed, 0.05f, 2.f));
	registerSetting(new SliderSetting<float>(
		"Vertical Speed", "Ascent and descent speed", &verticalSpeed,
		verticalSpeed, 0.05f, 2.f));
	registerSetting(new BoolSetting(
		"Auth Sync", "Send flight motion through authoritative input", &authSync, authSync));
}

std::string Fly::getModeText() {
	char text[24]{};
	sprintf_s(text, "H%.2f V%.2f", horizontalSpeed, verticalSpeed);
	return text;
}

bool Fly::runOnBackground() {
	return stopPending.load(std::memory_order_acquire);
}

void Fly::onEnable() {
	stopPending.store(false, std::memory_order_release);
}

void Fly::onDisable() {
	std::lock_guard lock(motionMutex);
	hasMotion = false;
	if (flightRequested)
		stopPending.store(true, std::memory_order_release);
}

void Fly::onTick(Level*) {
	LocalPlayer* player = Game::getLocalPlayer();
	if (player == nullptr || player->stateVector == nullptr || player->actorRotation == nullptr) {
		std::lock_guard lock(motionMutex);
		hasMotion = false;
		return;
	}

	Vec3<float>& velocity = player->getVelocity();
	velocity = Vec3<float>();
	if (Game::canUseMoveKeys()) {
		int* keyMap = reinterpret_cast<int*>(Game::keyMapPtr);
		const Keyboard keyboard(keyMap);
		const auto isKeyDown = [&](int key) {
			return keyMap != nullptr ? keyboard.isKeyDown(key) : (GetAsyncKeyState(key) & 0x8000) != 0;
		};
		float forward = static_cast<float>(isKeyDown('W')) - static_cast<float>(isKeyDown('S'));
		float strafe = static_cast<float>(isKeyDown('D')) - static_cast<float>(isKeyDown('A'));
		const float inputLength = std::hypot(forward, strafe);
		if (inputLength > 0.f) {
			forward /= inputLength;
			strafe /= inputLength;

			const float yaw = player->getRot().y * RAD_DEG;
			const float sine = std::sin(yaw);
			const float cosine = std::cos(yaw);
			velocity.x = (strafe * cosine - forward * sine) * horizontalSpeed;
			velocity.z = (forward * cosine + strafe * sine) * horizontalSpeed;
		}

		const float verticalInput = static_cast<float>(isKeyDown(VK_SPACE)) -
			static_cast<float>(isKeyDown(VK_SHIFT));
		velocity.y = verticalInput * verticalSpeed;
	}

	std::lock_guard lock(motionMutex);
	lastMotion = velocity;
	hasMotion = true;
}

void Fly::onSendPacket(Packet* packet, bool&) {
	if (packet == nullptr)
		return;
	PlayerAuthInputPacket* input = packet->as<PlayerAuthInputPacket>();
	if (input == nullptr)
		return;

	bool refreshCache = false;
	{
		std::lock_guard lock(motionMutex);
		if (stopPending.exchange(false, std::memory_order_acq_rel)) {
			input->setInputData(InputData::StartFlying, false);
			input->setInputData(InputData::StopFlying);
			flightRequested = false;
			refreshCache = true;
		}
		else if (!authSync) {
			if (flightRequested) {
				input->setInputData(InputData::StartFlying, false);
				input->setInputData(InputData::StopFlying);
				flightRequested = false;
			}
		}
		else if (isEnabled() && hasMotion) {
			input->mPosDelta = lastMotion;
			input->setInputData(InputData::Ascend, lastMotion.y > 0.f);
			input->setInputData(InputData::Descend, lastMotion.y < 0.f);
			if (!flightRequested) {
				input->setInputData(InputData::StopFlying, false);
				input->setInputData(InputData::StartFlying);
				flightRequested = true;
			}
		}
	}
	if (refreshCache) {
		extern void updateModuleCacheCallback();
		updateModuleCacheCallback();
	}
}

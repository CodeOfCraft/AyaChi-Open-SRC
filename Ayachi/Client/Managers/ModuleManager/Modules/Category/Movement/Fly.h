#pragma once

#include <atomic>
#include <mutex>

#include "../../ModuleBase/Module.h"

class Fly : public Module {
private:
	float horizontalSpeed = 0.35f;
	float verticalSpeed = 0.30f;
	bool authSync = true;
	std::mutex motionMutex;
	Vec3<float> lastMotion;
	bool hasMotion = false;
	bool flightRequested = false;
	std::atomic_bool stopPending = false;

public:
	Fly();

	std::string getModeText() override;
	bool runOnBackground() override;
	void onEnable() override;
	void onDisable() override;
	void onTick(Level* level) override;
	void onSendPacket(Packet* packet, bool& cancel) override;
};

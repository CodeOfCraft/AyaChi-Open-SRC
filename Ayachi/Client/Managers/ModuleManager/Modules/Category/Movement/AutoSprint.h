#pragma once

#include <atomic>

#include "../../ModuleBase/Module.h"

class AutoSprint : public Module {
private:
	std::atomic_bool sprintRequested = false;
	std::atomic_bool packetSprinting = false;
	std::atomic_bool cleanupPending = false;

public:
	AutoSprint();

	bool runOnBackground() override;
	void onDisable() override;
	void onTick(Level* level) override;
	void onSendPacket(Packet* packet, bool& cancel) override;
};

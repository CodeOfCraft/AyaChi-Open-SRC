#pragma once

#include "../../ModuleBase/Module.h"

class Velocity : public Module {
private:
	float horizontal = 0.f;
	float vertical = 0.f;

public:
	Velocity();

	std::string getModeText() override;
	void onReceivePacket(Packet* packet, bool& cancel) override;
};

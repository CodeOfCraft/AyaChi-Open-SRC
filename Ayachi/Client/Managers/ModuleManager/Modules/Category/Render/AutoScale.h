#pragma once

#include "../../ModuleBase/Module.h"

class AutoScale : public Module {
private:
	float scale = 2.5f;

public:
	AutoScale();
	void onTick(Level* level) override;
};

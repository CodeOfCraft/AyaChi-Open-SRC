#pragma once

#include "../../ModuleBase/Module.h"

class CustomFov : public Module {
private:
	float fov = 120.f;

public:
	CustomFov();
	void onGetFov(float& currentFov) override;
};

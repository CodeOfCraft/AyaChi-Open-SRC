#pragma once

#include "../../ModuleBase/Module.h"

class ItemPhysics : public Module {
public:
	float spinSpeed = 1.f;
	float groundOffset = 0.12f;
	float airOffset = 0.25f;

	ItemPhysics();
};

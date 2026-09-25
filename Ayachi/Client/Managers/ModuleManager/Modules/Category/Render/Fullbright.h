#pragma once

#include "../../ModuleBase/Module.h"

class Fullbright : public Module {
public:
	Fullbright();
	void onGetGamma(float& gamma) override;
};

#pragma once
#include "../../ModuleBase/Module.h"

class Notification : public Module {
public:
	Notification() : Module("Notification", "Controls notification display", Category::CLIENT) {}
};

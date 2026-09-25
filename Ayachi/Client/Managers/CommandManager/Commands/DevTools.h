#pragma once
#include "CommandBase.h"

class DevTools : public CommandBase {
public:
	DevTools();
	bool execute(const std::vector<std::string>& args);
};
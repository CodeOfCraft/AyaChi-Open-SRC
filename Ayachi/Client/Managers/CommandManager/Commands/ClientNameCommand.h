#pragma once
#include "CommandBase.h"

class ClientNameCommand : public CommandBase {
public:
	ClientNameCommand();
	bool execute(const std::vector<std::string>& args) override;
};
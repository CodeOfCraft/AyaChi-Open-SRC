#include "CommandBase.h"

CommandBase::CommandBase(std::string _name, std::string _description, std::string _usage, std::vector<std::string> _aliases)
	: name(std::move(_name)),
	  description(std::move(_description)),
	  usage(std::move(_usage)),
	  aliases(std::move(_aliases)) {}

bool CommandBase::execute(const std::vector<std::string>& args) {
	return true;
}

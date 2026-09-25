#include "ClientNameCommand.h"
#include "../../../Client.h"
#include "../../../../Utils/StringUtil.h"

ClientNameCommand::ClientNameCommand() : CommandBase("clientname", "changes the clients name", "<name>", { "clientname", "setname", "name" }) {

}

bool ClientNameCommand::execute(const std::vector<std::string>& args) {
	if (args.size() < 2)
		return false;
	const std::string name = StringUtil::join(args, 1);
	Client::setClientName(name);
	Client::DisplayClientMessage("%sSuccessfully changed client name to %s%s", MCTF::GREEN, MCTF::GOLD, name.c_str());
	return true;
}

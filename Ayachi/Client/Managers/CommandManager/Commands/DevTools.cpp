#include "DevTools.h"
#include "../../../Client.h"

DevTools::DevTools() : CommandBase("devtools", "commands for devs", "<devcommand> <optional argument>", { "devtool" }) {

}

bool DevTools::execute(const std::vector<std::string>& args) {
	Game::DisplayClientMessage("%sDevTools </>", MCTF::GRAY);
	Game::DisplayClientMessage("%sTemporarily unavailable (SDK rebuild in progress)", MCTF::GRAY);
	return true;
}

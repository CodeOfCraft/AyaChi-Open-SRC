#include "EjectCommand.h"
#include <Windows.h>

extern "C" DWORD WINAPI Ayachi_RequestEject(LPVOID);

EjectCommand::EjectCommand() : CommandBase("eject", "Removes the cheat from the game.", "", { "uninject" }) {
}

bool EjectCommand::execute(const std::vector<std::string>& args) {
	Ayachi_RequestEject(nullptr);
	return true;
}

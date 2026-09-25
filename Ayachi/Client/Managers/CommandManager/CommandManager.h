#pragma once

#include "Commands/BindCommand.h"
#include "Commands/ConfigCommand.h"
#include "Commands/EjectCommand.h"
#include "Commands/UnbindCommand.h"
#include "Commands/FriendCommand.h"
#include "Commands/ClientNameCommand.h"
#include "Commands/DevTools.h"
#include "Commands/ToggleCommand.h"

class CommandManager {
public:
	static inline std::vector<CommandBase*> commandList;
	static inline char prefix = '.';

	static void init();
	static void shutdown();
	static void execute(const std::string& message);
};
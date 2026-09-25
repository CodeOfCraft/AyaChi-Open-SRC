#include "CommandManager.h"
#include "../../Client.h"
#include "../../../Utils/StringUtil.h"

#include <optional>
#include <string_view>

namespace {
	std::optional<std::vector<std::string>> tokenize(std::string_view input) {
		std::vector<std::string> result;
		std::string current;
		bool quoted = false;
		bool escaped = false;

		for (char c : input) {
			if (escaped) {
				current.push_back(c);
				escaped = false;
				continue;
			}
			if (quoted && c == '\\') {
				escaped = true;
				continue;
			}
			if (c == '"') {
				quoted = !quoted;
				continue;
			}
			if (!quoted && std::isspace(static_cast<unsigned char>(c))) {
				if (!current.empty()) {
					result.push_back(std::move(current));
					current.clear();
				}
				continue;
			}
			current.push_back(c);
		}

		if (escaped)
			current.push_back('\\');
		if (quoted)
			return std::nullopt;
		if (!current.empty())
			result.push_back(std::move(current));
		return result;
	}
}

void CommandManager::init() {
	if (!commandList.empty())
		return;

	commandList.reserve(8);
	commandList.push_back(new BindCommand());
	commandList.push_back(new ConfigCommand());
	commandList.push_back(new EjectCommand());
	commandList.push_back(new UnbindCommand());
	commandList.push_back(new FriendCommand());
	commandList.push_back(new ClientNameCommand());
	commandList.push_back(new DevTools());
	commandList.push_back(new ToggleCommand());
}

void CommandManager::shutdown() {
	for (auto* command : commandList)
		delete command;
	commandList.clear();
}

void CommandManager::execute(const std::string& message) {
	if (message.empty() || message.front() != prefix)
		return;

	const auto parsed = tokenize(std::string_view(message).substr(1));
	if (!parsed.has_value()) {
		Client::DisplayClientMessage("%sCommand contains an unterminated quote.", MCTF::RED);
		return;
	}
	const std::vector<std::string>& args = *parsed;
	if (args.empty()) {
		Client::DisplayClientMessage("%sEnter a command after '%c'.", MCTF::GRAY, prefix);
		return;
	}

	const std::string& cmd = args[0];

	for (auto* command : commandList) {
		if (command == nullptr)
			continue;
		const bool matches = StringUtil::equalsIgnoreCase(cmd, command->name)
			|| std::any_of(command->aliases.begin(), command->aliases.end(),
				[&cmd](const std::string& alias) {
					return StringUtil::equalsIgnoreCase(cmd, alias);
				});
		if (!matches)
			continue;

		try {
			if (!command->execute(args)) {
				Client::DisplayClientMessage(
					"%sUsage: %s%c%s %s", MCTF::BOLD, MCTF::RESET,
					prefix, command->name.c_str(), command->usage.c_str());
			}
		} catch (const std::exception& error) {
			Client::DisplayClientMessage(
				"%sCommand '%s' failed: %.160s", MCTF::RED, command->name.c_str(), error.what());
		} catch (...) {
			Client::DisplayClientMessage("%sCommand '%s' failed.", MCTF::RED, command->name.c_str());
		}
		return;
	}

	Client::DisplayClientMessage("Couldn't find command %s%s%s!", MCTF::GRAY, cmd.c_str(), MCTF::WHITE);
}

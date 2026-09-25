#include "FriendCommand.h"
#include "../../FriendManager/FriendManager.h"
#include "../../../../SDK/Runtime/GameContext.h"
#include "../../../../SDK/World/Actor/Actor.h"
#include "../../../../SDK/World/Level/Level.h"
#include "../../../../Utils/StringUtil.h"

FriendCommand::FriendCommand() : CommandBase("friend", "Add/Remove people to friend list", "<add/remove> <name>", { "f" }) {
}

bool FriendCommand::execute(const std::vector<std::string>& args) {
	if (args.size() < 3)
		return false;
	const std::string requestedName = StringUtil::join(args, 2);
	if (requestedName.empty())
		return false;
	const std::string searchedName = StringUtil::toLower(requestedName);
	const std::string action = StringUtil::toLower(args[1]);
	if (action == "remove") {
		std::string trgtName;
		for (const std::string& name : FriendManager::getList()) {
			const std::string curName = StringUtil::toLower(name);
			if (curName.find(searchedName) != std::string::npos) {
				trgtName = name;
				break;
			}
		}
		if (!trgtName.empty()) {
			if (FriendManager::removeFromList(trgtName)) Game::DisplayClientMessage("%sRemoved %s%s%s from list.", MCTF::GREEN, MCTF::YELLOW, trgtName.c_str(), MCTF::GREEN);
			else Game::DisplayClientMessage("%s%s%s was not in your friendslist", MCTF::YELLOW, requestedName.c_str(), MCTF::RED);
			return true;
		}
		else {
			Game::DisplayClientMessage("%s%s%s was not in your friendslist", MCTF::YELLOW, requestedName.c_str(), MCTF::RED);
			return true;
		}
	}
	else if (action == "add") {
		std::string trgtName;
		Level* level = Game::getLevel();
		if (level != nullptr) {
			for (Actor* actor : level->getRuntimeActorList()) {
				if (actor == nullptr || !actor->isPlayer())
					continue;
				const std::string actorName = StringUtil::toLower(actor->getNameTag());
				if (actorName.find(searchedName) != std::string::npos) {
					trgtName = actor->getNameTag();
					break;
				}
			}
		}
		if (trgtName.empty())
			trgtName = requestedName;

		if (FriendManager::addToList(trgtName)) Game::DisplayClientMessage("%sAdded %s%s%s to list.", MCTF::GREEN, MCTF::YELLOW, trgtName.c_str(), MCTF::GREEN);
		else Game::DisplayClientMessage("%s%s%s is already in your friendslist", MCTF::YELLOW, trgtName.c_str(), MCTF::RED);
		return true;
	}
	return false;
}

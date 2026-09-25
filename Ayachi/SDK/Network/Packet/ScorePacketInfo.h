#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>

#include "../../World/Actor/ActorId.h"

class IdentityDefinition;

struct ScoreboardId {
	int64_t mRawID;
	IdentityDefinition* mIdentityDef;
};

struct PlayerScoreboardId {
	int64_t mActorUniqueId;
};

struct RemoveScore {
	ScoreboardId mScoreboardId;
	std::optional<std::string> mObjectiveName;
};

struct ChangePlayerScore {
	ScoreboardId mScoreboardId;
	std::string mObjectiveName;
	int32_t mScoreValue;
	PlayerScoreboardId mPlayerId;
};

struct ChangeEntityScore {
	ScoreboardId mScoreboardId;
	std::string mObjectiveName;
	int32_t mScoreValue;
	ActorUniqueID mEntityId;
};

struct ChangeFakePlayerScore {
	ScoreboardId mScoreboardId;
	std::string mObjectiveName;
	int32_t mScoreValue;
	std::string mFakePlayerName;
};

using ScorePacketInfo = std::variant<
	RemoveScore,
	ChangePlayerScore,
	ChangeEntityScore,
	ChangeFakePlayerScore>;

static_assert(sizeof(ScoreboardId) == 0x10);
static_assert(sizeof(PlayerScoreboardId) == 0x8);
static_assert(sizeof(RemoveScore) == 0x38);
static_assert(sizeof(ChangePlayerScore) == 0x40);
static_assert(sizeof(ChangeEntityScore) == 0x40);
static_assert(sizeof(ChangeFakePlayerScore) == 0x58);
static_assert(sizeof(ScorePacketInfo) == 0x60);

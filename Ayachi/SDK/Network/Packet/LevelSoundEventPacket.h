#pragma once

#include <optional>
#include <string>
#include <variant>

#include "Packet.h"
#include "../../World/Actor/ActorId.h"
#include "../../../Utils/Maths.h"

using SoundEventIdentifier = std::variant<std::monostate, int32_t, std::string>;

class LevelSoundEventPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::LevelSoundEvent;

	std::string mActorIdentifier;
	ActorUniqueID mActor;
	SoundEventIdentifier mSoundEvent;
	Vec3<float> mPos;
	int32_t mData;
	bool mIsGlobal;
	bool mIsBaby;
	std::optional<Vec3<float>> mFireAtPosition;
	int32_t mSerializationMode;
};

static_assert(sizeof(SoundEventIdentifier) == 0x28);
static_assert(sizeof(LevelSoundEventPacket) == 0xA0);

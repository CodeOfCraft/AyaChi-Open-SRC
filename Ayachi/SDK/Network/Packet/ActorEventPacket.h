#pragma once

#include <optional>

#include "Packet.h"
#include "../../World/Actor/ActorId.h"
#include "../../../Utils/Maths.h"

enum class ActorEvent : uint8_t {
	None = 0,
	Jump = 1,
	Hurt = 2,
	Death = 3,
	StartAttacking = 4,
	StopAttacking = 5,
	TamingFailed = 6,
	TamingSucceeded = 7,
	ShakeWetness = 8,
	EatGrass = 10,
	LoveHearts = 21,
	VillagerAngry = 22,
	VillagerHappy = 23,
	FireworksExplode = 25,
	PrimeCreeper = 32,
	StartSwimming = 70,
	ActorGrowUp = 76,
	DrinkMilk = 78,
};

class ActorEventPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::ActorEvent;

	ActorRuntimeID mRuntimeId;
	ActorEvent mEventId;
	int32_t mData;
	std::optional<Vec3<float>> mFireAtPosition;
	int32_t mSerializationMode;
};

static_assert(sizeof(ActorEventPacket) == 0x50);

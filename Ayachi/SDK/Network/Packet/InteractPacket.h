#pragma once

#include <optional>

#include "Packet.h"
#include "InteractionTypes.h"
#include "../../World/Actor/ActorId.h"
#include "../../../Utils/Maths.h"

class InteractPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::Interact;

	InteractAction mAction;
	ActorRuntimeID mTargetId;
	std::optional<Vec3<float>> mPos;
	int32_t mSerializationMode;
};

static_assert(sizeof(std::optional<Vec3<float>>) == 0x10);
static_assert(sizeof(InteractPacket) == 0x50);

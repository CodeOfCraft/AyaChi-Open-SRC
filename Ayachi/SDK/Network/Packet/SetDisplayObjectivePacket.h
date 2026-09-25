#pragma once

#include <string>

#include "Packet.h"

class SetDisplayObjectivePacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::SetDisplayObjective;

	std::string mDisplaySlotName;
	std::string mObjectiveName;
	std::string mObjectiveDisplayName;
	std::string mCriteriaName;
	int8_t mSortOrder;
	int32_t mSerializationMode;
};

static_assert(sizeof(SetDisplayObjectivePacket) == 0xB0);

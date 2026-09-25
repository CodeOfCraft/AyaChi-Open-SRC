#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "Packet.h"

struct LegacySetSlot {
	uint8_t mContainerEnum;
	std::array<std::byte, 7> mContainerPadding;
	std::vector<uint8_t> mSlots;
};

class InventoryTransactionPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::InventoryTransaction;

	std::array<std::byte, 16> mLegacyRequestId;
	std::vector<LegacySetSlot> mLegacySetItemSlots;
	std::array<std::byte, 272> mVariantTransaction;
	void* mTransaction;
	bool mIsClientSide;
	int32_t mSerializationMode;
};

static_assert(sizeof(LegacySetSlot) == 0x20);
static_assert(sizeof(InventoryTransactionPacket) == 0x170);

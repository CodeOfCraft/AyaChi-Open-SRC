#pragma once

#include <cstddef>
#include <string_view>

#include "PacketIDs.h"
#include "../../Runtime/GameData.h"
#include "../../../Utils/MemoryUtil.h"

class alignas(8) Packet {
public:
	static constexpr size_t BaseSize = 0x28;

	PacketID getId() const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Packet::getId, PacketID>(
			const_cast<Packet*>(this));
	}

	std::string_view getName() const {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Packet::getName, std::string_view>(
			const_cast<Packet*>(this));
	}

	void* getHandler() const {
		return *reinterpret_cast<void* const*>(reinterpret_cast<uintptr_t>(this) + 0x18);
	}

	template <typename T>
	bool is() const {
		return getId() == T::ID;
	}

	template <typename T>
	T* as() {
		return is<T>() ? reinterpret_cast<T*>(this) : nullptr;
	}

	template <typename T>
	const T* as() const {
		return is<T>() ? reinterpret_cast<const T*>(this) : nullptr;
	}

protected:
	std::byte mPacketStorage[BaseSize];
};

static_assert(sizeof(Packet) == Packet::BaseSize);
static_assert(alignof(Packet) == 8);

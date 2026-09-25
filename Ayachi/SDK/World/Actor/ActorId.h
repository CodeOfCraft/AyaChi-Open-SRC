#pragma once

#include <cstdint>

struct ActorRuntimeID {
	uint64_t rawID = 0;

	constexpr ActorRuntimeID() = default;
	constexpr ActorRuntimeID(uint64_t value) : rawID(value) {}
	constexpr operator uint64_t() const { return rawID; }
	constexpr bool operator==(const ActorRuntimeID&) const = default;
};

struct ActorUniqueID {
	int64_t rawID = 0;

	constexpr ActorUniqueID() = default;
	constexpr ActorUniqueID(int64_t value) : rawID(value) {}
	constexpr operator int64_t() const { return rawID; }
	constexpr bool operator==(const ActorUniqueID&) const = default;
};

static_assert(sizeof(ActorRuntimeID) == 0x8);
static_assert(sizeof(ActorUniqueID) == 0x8);

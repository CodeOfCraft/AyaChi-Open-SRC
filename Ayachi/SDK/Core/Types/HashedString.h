#pragma once

#include <cstdint>
#include <string>
#include <string_view>

class HashedString {
public:
	HashedString() = default;

	explicit HashedString(std::string_view value)
		: mStrHash(computeHash(value)), mStr(value), mLastMatch(nullptr) {}

	static constexpr uint64_t computeHash(std::string_view value) noexcept {
		if (value.empty())
			return 0;

		uint64_t hash = 0xCBF29CE484222325ULL;
		for (const char character : value)
			hash = static_cast<uint8_t>(character) ^ (0x100000001B3ULL * hash);
		return hash;
	}

	uint64_t getHash() const noexcept {
		return mStrHash;
	}

	const std::string& getString() const noexcept {
		return mStr;
	}

	const char* c_str() const noexcept {
		return mStr.c_str();
	}

	bool empty() const noexcept {
		return mStr.empty();
	}

	operator std::string_view() const noexcept {
		return mStr;
	}

private:
	uint64_t mStrHash = 0;
	std::string mStr;
	mutable const HashedString* mLastMatch = nullptr;
};

static_assert(sizeof(HashedString) == 0x30);

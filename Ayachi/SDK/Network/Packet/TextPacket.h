#pragma once

#include <optional>
#include <string>
#include <variant>

#include "Packet.h"
#include "TextPacketBody.h"

class TextPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::Text;

	bool mLocalize;
	std::string mXuid;
	std::string mPlatformId;
	std::optional<std::string> mFilteredMessage;
	TextPacketBody mBody;
	int32_t mSerializationMode;

	TextPacketType getType() const {
		return std::visit([](const auto& body) { return body.mType; }, mBody);
	}

	const std::string& getMessage() const {
		return std::visit([](const auto& body) -> const std::string& { return body.mMessage; }, mBody);
	}

	const std::string* getAuthor() const {
		const auto* body = std::get_if<TextPacketAuthorAndMessage>(&mBody);
		return body != nullptr ? &body->mAuthor : nullptr;
	}
};

static_assert(sizeof(TextPacket) == 0xF0);

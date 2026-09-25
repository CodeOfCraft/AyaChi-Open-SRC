#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

enum class TextPacketType : uint8_t {
	Raw = 0,
	Chat = 1,
	Translate = 2,
	Popup = 3,
	JukeboxPopup = 4,
	Tip = 5,
	SystemMessage = 6,
	Whisper = 7,
	Announcement = 8,
	TextObjectWhisper = 9,
	TextObject = 10,
	TextObjectAnnouncement = 11,
};

struct TextPacketMessageOnly {
	TextPacketType mType;
	std::string mMessage;
};

struct TextPacketAuthorAndMessage {
	TextPacketType mType;
	std::string mAuthor;
	std::string mMessage;
};

struct TextPacketMessageAndParams {
	TextPacketType mType;
	std::string mMessage;
	std::vector<std::string> mParams;
};

using TextPacketBody = std::variant<
	TextPacketMessageOnly,
	TextPacketAuthorAndMessage,
	TextPacketMessageAndParams>;

static_assert(sizeof(TextPacketBody) == 0x50);

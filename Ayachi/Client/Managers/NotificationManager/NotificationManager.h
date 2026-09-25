#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <imgui.h>
#include "../../../Renderer/ImGuiRender.h"

enum class NotifState : uint8_t {
	Entering,
	Active,
	Expiring
};

struct NotificationEntry {
	std::string message;
	float remaining     = 3.f;
	float totalDuration = 3.f;
	int   count         = 1;
	float posX          = 0.f;
	float posY          = 0.f;
	NotifState state    = NotifState::Entering;

	NotificationEntry(std::string msg, float lifetime = 3.f)
		: message(std::move(msg))
		, remaining(lifetime)
		, totalDuration(lifetime)
		, count(1)
		, posX(0.f)
		, posY(0.f)
		, state(NotifState::Entering)
	{}
};

namespace NotifStyle {
	constexpr float TEXT_SIZE        = 1.0f;
	constexpr float COUNT_TEXT_SIZE  = 0.9f;

	constexpr float PADDING_X        = 10.0f;
	constexpr float PADDING_Y        = 7.0f;
	constexpr float ROUNDING         = 4.0f;
	constexpr float GAP              = 6.0f;
	constexpr float ANCHOR_BOTTOM    = 50.0f;
	constexpr float MARGIN_RIGHT     = 10.0f;
	constexpr float OFFSCREEN_PAD    = 20.0f;

	constexpr float SLIDE_SPEED      = 10.0f;
	constexpr float SNAP_SPEED       = 12.0f;
	constexpr float FADE_DISTANCE    = 50.0f;
	constexpr float ARRIVE_EPSILON   = 0.5f;

	constexpr uint8_t ALPHA_SHADOW          = 40;
	constexpr uint8_t ALPHA_BACKGROUND      = 140;
	constexpr uint8_t ALPHA_BORDER          = 80;
	constexpr uint8_t ALPHA_PROGRESS_FILL   = 120;
	constexpr uint8_t ALPHA_TEXT_SHADOW     = 100;
	constexpr uint8_t ALPHA_COUNT           = 200;
	constexpr uint8_t ALPHA_CLIP            = 5;

	constexpr uint8_t BG_R = 18, BG_G = 22, BG_B = 30;
	constexpr uint8_t BORDER_R = 62, BORDER_G = 76, BORDER_B = 88;
	constexpr uint8_t TEXT_R = 236, TEXT_G = 241, TEXT_B = 245;
	constexpr uint8_t FALLBACK_R = 78, FALLBACK_G = 221, FALLBACK_B = 196;
}

class NotificationManager {
private:
	static inline std::vector<std::shared_ptr<NotificationEntry>> notifList;
	static inline std::mutex mtx;

public:
	static void addNotifBox(std::string message, float duration = 3.f);
	static void Render(ImDrawList* drawlist);
};

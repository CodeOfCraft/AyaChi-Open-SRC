#include "NotificationManager.h"
#include "../ModuleManager/ModuleManager.h"
#include "../ModuleManager/Modules/Category/Client/Colors.h"
#include "../ModuleManager/Modules/Category/Client/Notification.h"

void NotificationManager::addNotifBox(std::string message, float duration) {
	auto* notificationModule = ModuleManager::getModule<Notification>();
	if (notificationModule && !notificationModule->isEnabled())
		return;

	std::lock_guard<std::mutex> lock(mtx);
	for (auto& notification : notifList) {
		if (notification->message == message) {
			++notification->count;
			notification->remaining = duration;
			notification->totalDuration = duration;
			return;
		}
	}

	auto notification = std::make_shared<NotificationEntry>(std::move(message), duration);
	if (Game::clientInstance && Game::clientInstance->getGuiData()) {
		notification->posX = Game::clientInstance->getGuiData()->screenSize.x
			+ NotifStyle::OFFSCREEN_PAD;
		notification->posY = Game::clientInstance->getGuiData()->screenSize.y;
	}
	notifList.push_back(std::move(notification));
}

void NotificationManager::Render(ImDrawList* drawlist) {
	if (!drawlist || !Game::clientInstance || !Game::clientInstance->getGuiData())
		return;

	auto* notificationModule = ModuleManager::getModule<Notification>();
	if (notificationModule && !notificationModule->isEnabled()) {
		std::lock_guard<std::mutex> lock(mtx);
		notifList.clear();
		return;
	}

	using namespace NotifStyle;

	const float deltaTime = ImGui::GetIO().DeltaTime;
	const Vec2<float> screen = Game::clientInstance->getGuiData()->screenSize;
	const float textHeight = ImGuiRender::getTextHeight(TEXT_SIZE);
	const float rowHeight = textHeight + PADDING_Y * 2.f;
	const float anchorX = screen.x - MARGIN_RIGHT;

	struct RenderEntry {
		std::string message;
		std::string countText;
		float messageWidth;
		float boxWidth;
		float remaining;
		float totalDuration;
		float posX;
		float posY;
		NotifState state;
	};

	std::vector<RenderEntry> renderList;
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (notifList.empty())
			return;

		renderList.reserve(notifList.size());
		float anchorY = screen.y - ANCHOR_BOTTOM;
		for (auto& notification : notifList) {
			const bool hasCount = notification->count > 1;
			std::string countText = hasCount ? " x" + std::to_string(notification->count) : "";
			const float messageWidth = ImGuiRender::getTextWidth(notification->message, TEXT_SIZE);
			const float countWidth = hasCount
				? ImGuiRender::getTextWidth(countText, COUNT_TEXT_SIZE) + 6.f
				: 0.f;
			const float boxWidth = messageWidth + countWidth + PADDING_X * 2.f;
			const float visibleX = anchorX - boxWidth;
			const float targetX = notification->state == NotifState::Expiring
				? screen.x + OFFSCREEN_PAD
				: visibleX;
			const float targetY = anchorY - rowHeight;

			notification->posX += (targetX - notification->posX)
				* std::min(deltaTime * SLIDE_SPEED, 1.f);
			notification->posY += (targetY - notification->posY)
				* std::min(deltaTime * SNAP_SPEED, 1.f);

			switch (notification->state) {
			case NotifState::Entering:
				if (std::abs(notification->posX - targetX) < ARRIVE_EPSILON)
					notification->state = NotifState::Active;
				break;
			case NotifState::Active:
				notification->remaining -= deltaTime;
				if (notification->remaining <= 0.f) {
					notification->remaining = 0.f;
					notification->state = NotifState::Expiring;
				}
				break;
			case NotifState::Expiring:
				break;
			}

			if (notification->state != NotifState::Expiring || notification->posX <= screen.x) {
				renderList.push_back({
					notification->message,
					std::move(countText),
					messageWidth,
					boxWidth,
					notification->remaining,
					notification->totalDuration,
					notification->posX,
					notification->posY,
					notification->state
				});
			}

			anchorY -= rowHeight + GAP;
		}

		std::erase_if(notifList, [&screen](const auto& notification) {
			return notification->state == NotifState::Expiring && notification->posX > screen.x;
		});
	}

	auto* colorsModule = ModuleManager::getModule<Colors>();
	const UIColor themeColor = colorsModule
		? colorsModule->getColor()
		: UIColor(FALLBACK_R, FALLBACK_G, FALLBACK_B);

	for (const auto& notification : renderList) {
		if (notification.posX > screen.x)
			continue;

		float alphaFactor = 1.f;
		if (notification.state == NotifState::Entering) {
			const float distance = notification.posX - (anchorX - notification.boxWidth);
			alphaFactor = 1.f - std::clamp(distance / FADE_DISTANCE, 0.f, 1.f);
		}

		const int alpha = static_cast<int>(255.f * alphaFactor);
		if (alpha < ALPHA_CLIP)
			continue;

		const Vec4<float> bounds(notification.posX, notification.posY,
			notification.posX + notification.boxWidth, notification.posY + rowHeight);
		drawlist->AddRectFilled(
			ImVec2(bounds.x + 2.f, bounds.y + 2.f),
			ImVec2(bounds.z + 2.f, bounds.w + 2.f),
			IM_COL32(0, 0, 0, static_cast<int>(ALPHA_SHADOW * alphaFactor)), ROUNDING);

		ImGuiRender::drawBlur(bounds, ROUNDING, 0.78f);
		drawlist->AddRectFilled(
			ImVec2(bounds.x, bounds.y),
			ImVec2(bounds.z, bounds.w),
			IM_COL32(BG_R, BG_G, BG_B, static_cast<int>(ALPHA_BACKGROUND * alphaFactor)), ROUNDING);
		drawlist->AddRect(
			ImVec2(bounds.x, bounds.y),
			ImVec2(bounds.z, bounds.w),
			IM_COL32(BORDER_R, BORDER_G, BORDER_B, static_cast<int>(ALPHA_BORDER * alphaFactor)),
			ROUNDING, 0, 1.f);

		if (notification.totalDuration > 0.f && notification.state != NotifState::Entering) {
			const float progress = std::max(0.f, notification.remaining) / notification.totalDuration;
			drawlist->AddRectFilled(
				ImVec2(bounds.x, bounds.y),
				ImVec2(bounds.x + notification.boxWidth * progress, bounds.w),
				IM_COL32(themeColor.r, themeColor.g, themeColor.b,
					static_cast<int>(ALPHA_PROGRESS_FILL * alphaFactor)), ROUNDING);
		}

		const float textY = notification.posY + (rowHeight - textHeight) / 2.f;
		const float textX = notification.posX + PADDING_X;
		ImGuiRender::drawText(
			Vec2<float>(textX + 1.f, textY + 1.f), notification.message,
			UIColor(0, 0, 0, static_cast<int>(ALPHA_TEXT_SHADOW * alphaFactor)),
			TEXT_SIZE, false);
		ImGuiRender::drawText(
			Vec2<float>(textX, textY), notification.message,
			UIColor(TEXT_R, TEXT_G, TEXT_B, alpha), TEXT_SIZE, false);

		if (!notification.countText.empty()) {
			const float countTextHeight = ImGuiRender::getTextHeight(COUNT_TEXT_SIZE);
			const float countX = textX + notification.messageWidth + 3.f;
			const float countY = notification.posY + (rowHeight - countTextHeight) / 2.f;
			ImGuiRender::drawText(
				Vec2<float>(countX, countY), notification.countText,
				UIColor(255, 255, 255, static_cast<int>(ALPHA_COUNT * alphaFactor)),
				COUNT_TEXT_SIZE, false);
		}
	}
}

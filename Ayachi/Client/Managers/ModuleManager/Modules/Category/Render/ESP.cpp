#include "ESP.h"

#include <cmath>

#include "../../../../../../Renderer/ImGuiRender.h"
#include "../../../../../../SDK/World/Actor/Actor.h"
#include "../../../../../../SDK/World/Actor/Player/LocalPlayer.h"
#include "../../../../../../SDK/World/Level/Level.h"
#include "../../../ModuleManager.h"
#include "../Client/ClickGUI.h"
#include "../Client/HudEditor.h"

namespace {
	bool isValidBox(const AABB& box) {
		const Vec3<float> size = box.upper.sub(box.lower);
		return std::isfinite(box.lower.x) && std::isfinite(box.lower.y) && std::isfinite(box.lower.z) &&
			std::isfinite(box.upper.x) && std::isfinite(box.upper.y) && std::isfinite(box.upper.z) &&
			size.x > 0.f && size.y > 0.f && size.z > 0.f &&
			size.x < 1024.f && size.y < 1024.f && size.z < 1024.f;
	}
}

ESP::ESP()
	: Module("ESP", "Highlight players and selected entity types", Category::RENDER) {
	registerSetting(new ColorSetting("Color", "ESP color", &color, UIColor(255, 255, 255), false));
	registerSetting(new SliderSetting<int>("Alpha", "Fill opacity", &alpha, 40, 0, 255));
	registerSetting(new SliderSetting<int>("LineAlpha", "Outline opacity", &lineAlpha, 135, 0, 255));
	registerSetting(new BoolSetting("Mobs", "Include mobs", &mobs, false));
	registerSetting(new BoolSetting("Items", "Include dropped items", &items, false));
}

void ESP::onDisable() {
	std::scoped_lock lock(snapshotMutex);
	boxSnapshot.clear();
	snapshotTime = {};
}

void ESP::onLevelRender(LevelRenderer*, ScreenContext*) {
	LocalPlayer* localPlayer = Game::getLocalPlayer();
	Level* level = Game::getLevel();
	if (localPlayer == nullptr || level == nullptr)
		return;

	std::vector<AABB> nextSnapshot;
	for (Actor* actor : level->getRuntimeActorList()) {
		if (actor == nullptr || actor == localPlayer || actor->removed || actor->aabbShape == nullptr)
			continue;

		const bool shouldRender = actor->hasCategory(ActorCategory::Player) ||
			(mobs && actor->hasCategory(ActorCategory::Mob)) ||
			(items && actor->hasCategory(ActorCategory::Item));
		if (!shouldRender)
			continue;

		const AABB box = actor->getBoundingBox();
		if (isValidBox(box))
			nextSnapshot.push_back(box);
	}

	std::scoped_lock lock(snapshotMutex);
	boxSnapshot = std::move(nextSnapshot);
	snapshotTime = std::chrono::steady_clock::now();
}

void ESP::onImGuiRender(ImDrawList* drawlist) {
	ClientInstance* clientInstance = Game::getClientInstance();
	static ClickGUI* clickGui = ModuleManager::getModule<ClickGUI>();
	static HudEditor* hudEditor = ModuleManager::getModule<HudEditor>();
	if (drawlist == nullptr || clientInstance == nullptr ||
		!clientInstance->isInWorldAndNotShowingAnyMenuScreens() ||
		(clickGui != nullptr && clickGui->isEnabled()) ||
		(hudEditor != nullptr && hudEditor->isEnabled()))
		return;

	std::vector<AABB> boxes;
	{
		std::scoped_lock lock(snapshotMutex);
		if (snapshotTime.time_since_epoch().count() == 0 ||
			std::chrono::steady_clock::now() - snapshotTime > std::chrono::milliseconds(250))
			return;
		boxes = boxSnapshot;
	}

	const UIColor fill(color.r, color.g, color.b, static_cast<uint8_t>(alpha));
	const UIColor line(color.r, color.g, color.b, static_cast<uint8_t>(lineAlpha));
	for (const AABB& box : boxes)
		ImGuiRender::drawBox3D(drawlist, box, fill, line);
}

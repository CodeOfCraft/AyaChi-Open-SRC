#include "HudEditor.h"
#include "ClickGUI.h"
#include "Colors.h"
#include "../../../../../../Renderer/ImGuiRender.h"
#include "../../../../../Client.h"
#include "../../../../../../Libs/json.hpp"

HudEditor::HudEditor() : Module("HudEditor", "Drag to reposition HUD elements", Category::CLIENT) {
	if (elements.empty()) {
		elements.push_back({ "Watermark",  "Watermark",  Vec2<float>(0.005f, 0.005f), Vec2<float>(180.f, 22.f), true });
		elements.push_back({ "FPS",        "FPS",        Vec2<float>(0.005f, 0.92f),  Vec2<float>(100.f, 20.f), true });
		elements.push_back({ "Position",   "Position",   Vec2<float>(0.005f, 0.88f),  Vec2<float>(260.f, 20.f), true });
		elements.push_back({ "Direction",  "Direction",  Vec2<float>(0.005f, 0.85f),  Vec2<float>(160.f, 20.f), true });
		elements.push_back({ "Ping",       "Ping",       Vec2<float>(0.005f, 0.82f),  Vec2<float>(120.f, 20.f), true });
		elements.push_back({ "Item",        "Item",        Vec2<float>(0.53f, 0.946f), Vec2<float>(80.f, 20.f), true });
		elements.push_back({ "Effects",      "Effects",      Vec2<float>(0.912f, 0.005f), Vec2<float>(160.f, 20.f), true });
		elements.push_back({ "Keystrokes",   "Keystrokes",   Vec2<float>(0.005f, 0.72f),  Vec2<float>(110.f, 110.f), Vec2<float>(0.f, 0.f), true, false });
		elements.push_back({ "Minimap",      "Minimap",      Vec2<float>(0.005f, 0.005f), Vec2<float>(128.f, 128.f), true });
		elements.push_back({ "InvView",      "InvView",      Vec2<float>(0.02f, 0.15f),  Vec2<float>(160.f, 60.f),  true });
		elements.push_back({ "Arraylist",    "Arraylist",    Vec2<float>(0.82f, 0.02f),  Vec2<float>(220.f, 180.f), Vec2<float>(0.f, 0.f), false, false });
	}
}

void HudEditor::onEnable() {
	if (!Game::clientInstance) return;

	static ClickGUI* clickGuiMod = ModuleManager::getModule<ClickGUI>();
	if (clickGuiMod && clickGuiMod->isEnabled()) {
		clickGuiMod->setEnabled(false);
	}

	Mouse::refreshOwner();
	draggingIndex = -1;
}

void HudEditor::onDisable() {
	if (!Game::clientInstance) return;
	draggingIndex = -1;
	Mouse::refreshOwner();
}

void HudEditor::onKeyUpdate(int key, bool isDown) {
	if (isDown && key == VK_ESCAPE) {
		setEnabled(false);
		return;
	}
}

void HudEditor::onImGuiRender(ImDrawList* drawlist) {
	if (!this->isEnabled()) return;

	const Vec2<float>& mousePos = Mouse::pos();

	Vec2<float> screenSize = Game::clientInstance->getGuiData()->screenSize;

	static ClickGUI* clickGuiMod = ModuleManager::getModule<ClickGUI>();
	wasClickGUIOpen = clickGuiMod && clickGuiMod->isEnabled();

	ImGuiRender::fillRectangle(Vec4<float>(0.f, 0.f, screenSize.x, screenSize.y),
		UIColor(0, 0, 0, 25));

	std::string titleText = "HUD Editor - Drag elements to reposition | ESC or keybind to exit";
	float titleW = ImGuiRender::getTextWidth(titleText, 1.2f);
	Vec2<float> titlePos = Vec2<float>((screenSize.x - titleW) / 2.f, 10.f);
	ImGuiRender::drawText(titlePos, titleText, UIColor(255, 255, 255, 200), 1.2f, false);

	static Colors* colorsMod = ModuleManager::getModule<Colors>();

	ImGuiRender::drawLine(Vec2<float>(screenSize.x / 2.f, screenSize.y / 2.f - 10.f),
		Vec2<float>(screenSize.x / 2.f, screenSize.y / 2.f + 10.f),
		UIColor(255, 255, 255, 50), 1.f);
	ImGuiRender::drawLine(Vec2<float>(screenSize.x / 2.f - 10.f, screenSize.y / 2.f),
		Vec2<float>(screenSize.x / 2.f + 10.f, screenSize.y / 2.f),
		UIColor(255, 255, 255, 50), 1.f);

	for (int i = 0; i < elements.size(); i++) {
		auto& elem = elements[i];
		if (!elem.visible) continue;

		Vec2<float> pixelPos = Vec2<float>(
			elem.position.x * screenSize.x,
			elem.position.y * screenSize.y
		);

		float elemWidth, elemHeight;
		if (elem.actualSize.x > 0.f && elem.actualSize.y > 0.f) {
			elemWidth = elem.actualSize.x;
			elemHeight = elem.actualSize.y;
		}
		else {
			elemWidth = elem.defSize.x;
			elemHeight = elem.defSize.y;

			if (elem.id == "Watermark") {
				elemWidth = ImGuiRender::getTextWidth(Client::getClientName() + " | User | v" + Client::getVersion(), 1.1f) + 12.f;
				elemHeight = ImGuiRender::getTextHeight(1.1f) + 8.f;
			}
			else if (elem.id == "FPS" || elem.id == "Ping" || elem.id == "Direction") {
				elemWidth = ImGuiRender::getTextWidth(elem.label + ": 000", 1.f) + 12.f;
				elemHeight = ImGuiRender::getTextHeight(1.f) + 6.f;
			}
			else if (elem.id == "Position") {
				elemWidth = ImGuiRender::getTextWidth("Position: -000.0, -000.0, -000.0", 1.f) + 12.f;
				elemHeight = ImGuiRender::getTextHeight(1.f) + 6.f;
			}
			else if (elem.id == "Effects") {
				elemWidth = ImGuiRender::getTextWidth("Speed 99:99", 1.f) + 12.f;
				elemHeight = ImGuiRender::getTextHeight(1.f) + 6.f;
			}
			else if (elem.id == "Keystrokes") {
				elemWidth = 110.f;
				elemHeight = 110.f;
			}
			else if (elem.id == "Armor") {
				elemWidth = 140.f;
				elemHeight = 30.f;
			}
			else if (elem.id == "Minimap") {
				elemWidth = 128.f;
				elemHeight = 128.f;
			}
			else if (elem.id == "InvView") {
				elemWidth = 160.f;
				elemHeight = 60.f;
			}
			}

		elem.defSize = Vec2<float>(elemWidth, elemHeight);

		Vec4<float> rect = Vec4<float>(
			pixelPos.x, pixelPos.y,
			pixelPos.x + elemWidth, pixelPos.y + elemHeight
		);

		bool isHovered = rect.contains(mousePos);
		bool isDragging = (draggingIndex == i);

		UIColor accentColor = colorsMod ? colorsMod->getColor(-i * 50) : UIColor(78, 221, 196, 200);

		if (isDragging) {
			ImGuiRender::fillRectangle(rect, UIColor(accentColor.r, accentColor.g, accentColor.b, 80), 4.f);
			ImGuiRender::drawRectangle(rect, accentColor, 2.f, 4.f);

			float snapX = pixelPos.x + elemWidth / 2.f;
			float snapY = pixelPos.y + elemHeight / 2.f;
			if (std::abs(snapX - screenSize.x / 2.f) < 5.f) {
				ImGuiRender::drawLine(Vec2<float>(screenSize.x / 2.f, 0.f),
					Vec2<float>(screenSize.x / 2.f, screenSize.y),
					UIColor(255, 255, 100, 100), 1.f);
			}
			if (std::abs(snapY - screenSize.y / 2.f) < 5.f) {
				ImGuiRender::drawLine(Vec2<float>(0.f, screenSize.y / 2.f),
					Vec2<float>(screenSize.x, screenSize.y / 2.f),
					UIColor(255, 255, 100, 100), 1.f);
			}

			if (snappedV) {
				ImGuiRender::drawLine(Vec2<float>(snapLineX, 0.f),
					Vec2<float>(snapLineX, screenSize.y),
					UIColor(0, 255, 255, 140), 1.5f);
			}
			if (snappedH) {
				ImGuiRender::drawLine(Vec2<float>(0.f, snapLineY),
					Vec2<float>(screenSize.x, snapLineY),
					UIColor(0, 255, 255, 140), 1.5f);
			}

			char posText[80];
			sprintf_s(posText, "%.0f, %.0f  (%.3f, %.3f)",
				pixelPos.x, pixelPos.y,
				elem.position.x, elem.position.y);
			float posTextW = ImGuiRender::getTextWidth(posText, 0.9f);
			ImGuiRender::fillRectangle(
				Vec4<float>(pixelPos.x + elemWidth / 2.f - posTextW / 2.f - 4.f,
					pixelPos.y + elemHeight + 4.f,
					pixelPos.x + elemWidth / 2.f + posTextW / 2.f + 4.f,
					pixelPos.y + elemHeight + 4.f + ImGuiRender::getTextHeight(0.9f) + 4.f),
				UIColor(0, 0, 0, 200), 3.f);
			ImGuiRender::drawText(
				Vec2<float>(pixelPos.x + elemWidth / 2.f - posTextW / 2.f,
					pixelPos.y + elemHeight + 6.f),
				posText, UIColor(255, 255, 255, 255), 0.9f);
		}
		else if (isHovered) {
			ImGuiRender::fillRectangle(rect, UIColor(accentColor.r, accentColor.g, accentColor.b, 30), 4.f);
			ImGuiRender::drawRectangle(rect, accentColor, 1.5f, 4.f);
		}
		else {
			ImGuiRender::drawRectangle(rect, UIColor(255, 255, 255, 60), 1.f, 4.f);
		}

		float labelW = ImGuiRender::getTextWidth(elem.label, 0.9f);
		float labelHgt = ImGuiRender::getTextHeight(0.9f);
		Vec2<float> labelPos = Vec2<float>(
			pixelPos.x + (elemWidth - labelW) / 2.f,
			pixelPos.y + (elemHeight - labelHgt) / 2.f
		);
		ImGuiRender::drawText(labelPos, elem.label,
			isDragging ? UIColor(255, 255, 255) : UIColor(200, 200, 200),
			0.9f, false);

		if (isHovered && draggingIndex == -1 && Mouse::leftClicked()) {
			Mouse::consumeLeft();
			draggingIndex = i;
			dragOffset = Vec2<float>(
				mousePos.x - pixelPos.x,
				mousePos.y - pixelPos.y
			);
		}
	}

	if (draggingIndex >= 0 && draggingIndex < elements.size()) {
		if (Mouse::leftHeld()) {
			auto& elem = elements[draggingIndex];
			elem.customPosition = true;

			Vec2<float> newPixelPos = Vec2<float>(
				mousePos.x - dragOffset.x,
				mousePos.y - dragOffset.y
			);

			float snapDist = 8.f;
			float centerX = newPixelPos.x + elem.defSize.x / 2.f;
			float centerY = newPixelPos.y + elem.defSize.y / 2.f;
			if (std::abs(centerX - screenSize.x / 2.f) < snapDist)
				newPixelPos.x = screenSize.x / 2.f - elem.defSize.x / 2.f;
			if (std::abs(centerY - screenSize.y / 2.f) < snapDist)
				newPixelPos.y = screenSize.y / 2.f - elem.defSize.y / 2.f;

			float elemSnapDist = 10.f;
			float bestHdist = elemSnapDist;
			float bestVdist = elemSnapDist;
			float hTarget = newPixelPos.x;
			float vTarget = newPixelPos.y;
			snappedH = false;
			snappedV = false;

			for (int j = 0; j < elements.size(); j++) {
				if (j == draggingIndex || !elements[j].visible) continue;
				auto& other = elements[j];

				Vec2<float> oPos = Vec2<float>(
					other.position.x * screenSize.x,
					other.position.y * screenSize.y
				);

				float dL = newPixelPos.x;
				float dR = newPixelPos.x + elem.defSize.x;
				float dC = newPixelPos.x + elem.defSize.x / 2.f;
				float dT = newPixelPos.y;
				float dB = newPixelPos.y + elem.defSize.y;
				float dM = newPixelPos.y + elem.defSize.y / 2.f;

				float oL = oPos.x;
				float oR = oPos.x + other.defSize.x;
				float oC = oPos.x + other.defSize.x / 2.f;
				float oT = oPos.y;
				float oB = oPos.y + other.defSize.y;
				float oM = oPos.y + other.defSize.y / 2.f;

				auto tryHSnap = [&](float drEdge, float otEdge, float targetX) {
					float dist = std::abs(drEdge - otEdge);
					if (dist < bestHdist) { bestHdist = dist; hTarget = targetX; snappedH = true; snapLineX = otEdge; }
				};
				tryHSnap(dL, oL, oL);
				tryHSnap(dL, oR, oR);
				tryHSnap(dR, oL, oL - elem.defSize.x);
				tryHSnap(dR, oR, oR - elem.defSize.x);
				tryHSnap(dC, oC, oC - elem.defSize.x / 2.f);

				auto tryVSnap = [&](float drEdge, float otEdge, float targetY) {
					float dist = std::abs(drEdge - otEdge);
					if (dist < bestVdist) { bestVdist = dist; vTarget = targetY; snappedV = true; snapLineY = otEdge; }
				};
				tryVSnap(dT, oT, oT);
				tryVSnap(dT, oB, oB);
				tryVSnap(dB, oT, oT - elem.defSize.y);
				tryVSnap(dB, oB, oB - elem.defSize.y);
				tryVSnap(dM, oM, oM - elem.defSize.y / 2.f);
			}

			if (snappedH) newPixelPos.x = hTarget;
			if (snappedV) newPixelPos.y = vTarget;

			newPixelPos.x = std::clamp(newPixelPos.x, 0.f, screenSize.x - elem.defSize.x);
			newPixelPos.y = std::clamp(newPixelPos.y, 0.f, screenSize.y - elem.defSize.y);

			elem.position = Vec2<float>(
				newPixelPos.x / screenSize.x,
				newPixelPos.y / screenSize.y
			);
		}
		else {
			draggingIndex = -1;
		}
	}
}

void HudEditor::onLoadConfig(void* conf) {
	Module::onLoadConfig(conf);

	json* config = static_cast<json*>(conf);
	if (config->contains("hudPositions")) {
		auto& posJson = (*config)["hudPositions"];
		for (auto& elem : elements) {
			if (posJson.contains(elem.id)) {
				elem.position.x = posJson[elem.id]["x"].get<float>();
				elem.position.y = posJson[elem.id]["y"].get<float>();
				elem.customPosition = true;
			}
		}
	}
}

void HudEditor::onSaveConfig(void* conf) {
	Module::onSaveConfig(conf);

	json* config = static_cast<json*>(conf);
	json posJson = json::object();
	for (auto& elem : elements) {
		posJson[elem.id] = {
			{"x", elem.position.x},
			{"y", elem.position.y}
		};
	}
	(*config)["hudPositions"] = posJson;
}

Vec2<float> HudEditor::getPosition(const std::string& elementId) {
	for (auto& elem : elements) {
		if (elem.id == elementId)
			return elem.position;
	}
	return Vec2<float>(-1.f, -1.f);
}

bool HudEditor::hasCustomPosition(const std::string& elementId) {
	for (auto& elem : elements) {
		if (elem.id == elementId)
			return elem.customPosition;
	}
	return false;
}

void HudEditor::setVisible(const std::string& elementId, bool vis) {
	for (auto& elem : elements) {
		if (elem.id == elementId) {
			elem.visible = vis;
			return;
		}
	}
}

void HudEditor::setActualSize(const std::string& elementId, Vec2<float> size) {
	for (auto& elem : elements) {
		if (elem.id == elementId) {
			elem.actualSize = size;
			return;
		}
	}
}

void HudEditor::setDefaultPosition(const std::string& elementId, Vec2<float> pixelPos, Vec2<float> screenSize) {
	for (auto& elem : elements) {
		if (elem.id == elementId) {
			if (!elem.customPosition) {
				elem.position = Vec2<float>(pixelPos.x / screenSize.x, pixelPos.y / screenSize.y);
			}
			return;
		}
	}
}

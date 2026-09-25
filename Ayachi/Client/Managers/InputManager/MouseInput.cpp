#include "MouseInput.h"

#include <atomic>
#include <mutex>

#include "../ModuleManager/ModuleManager.h"
#include "../ModuleManager/Modules/Category/Client/ClickGUI.h"
#include "../ModuleManager/Modules/Category/Client/HudEditor.h"
#include "../../Client.h"
#include "../../../SDK/Runtime/GameContext.h"
#include "../../../SDK/Core/Client/MinecraftGame.h"
#include "../../../SDK/Runtime/GameData.h"
#include "../../../SDK/Input/MouseDevice.h"

namespace {

	constexpr int kButtonCount = static_cast<int>(Mouse::Button::Count);
	constexpr int kMaxEvents = 512;

	enum class EventType : uint8_t {
		Move = 0,
		ButtonState = 1,
		Wheel = 2,
		ResetButtons = 3
	};

	struct Event {
		EventType type;
		uint8_t button;
		uint8_t isDown;
		float x;
		float y;
		float wheel;
	};

	std::mutex g_queueMutex;
	Event g_queue[kMaxEvents];
	int g_queueCount = 0;

	Vec2<float> g_pos(0.f, 0.f);
	Vec2<float> g_delta(0.f, 0.f);
	bool g_down[kButtonCount] = {};
	bool g_pressed[kButtonCount] = {};
	bool g_released[kButtonCount] = {};
	float g_wheel = 0.f;

	std::atomic<Mouse::Owner> g_owner{ Mouse::Owner::Game };

	std::atomic<bool> g_guiOwns{ false };
	std::atomic<bool> g_wantCursorReleased{ false };
	std::atomic<bool> g_cursorReleasedByUs{ false };
	std::mutex g_cursorMutex;
	std::atomic<float> g_scaleX{ 1.f };
	std::atomic<float> g_scaleY{ 1.f };

	Vec2<float> g_lastDevicePos(0.f, 0.f);
	bool g_lastDevicePosValid = false;
	bool g_lastDeviceButtons[kButtonCount] = {};

	inline int buttonIndex(Mouse::Button button) {
		const int index = static_cast<int>(button);
		return (index >= 0 && index < kButtonCount) ? index : 0;
	}

	void enqueue(const Event& event) {
		std::lock_guard<std::mutex> lock(g_queueMutex);
		if (g_queueCount >= kMaxEvents) {
			if (event.type == EventType::ResetButtons)
				g_queue[kMaxEvents - 1] = event;
			return;
		}
		g_queue[g_queueCount++] = event;
	}

	void pollDevicePosition(bool& gotPos, Vec2<float>& newPos) {
		MouseDevice* device = MouseDevice::get();
		if (device == nullptr)
			return;

		const Vec2<float> devicePos((float)device->x, (float)device->y);
		const bool moved = !g_lastDevicePosValid
			|| fabsf(devicePos.x - g_lastDevicePos.x) > 0.5f
			|| fabsf(devicePos.y - g_lastDevicePos.y) > 0.5f;
		if (!moved)
			return;

		g_lastDevicePos = devicePos;
		g_lastDevicePosValid = true;
		if (!gotPos) {
			newPos = devicePos;
			gotPos = true;
		}
	}

	void pollDeviceButtons() {
		if (Addresses::GameCore_handleMouseInput != 0)
			return;

		MouseDevice* device = MouseDevice::get();
		if (device == nullptr)
			return;

		for (int i = 0; i < 2; i++) {
			const bool state = device->buttonStates[i];
			if (state == g_lastDeviceButtons[i])
				continue;
			g_lastDeviceButtons[i] = state;
			g_down[i] = state;
			if (state)
				g_pressed[i] = true;
			else
				g_released[i] = true;
		}
	}
}

namespace Mouse {

	void pushMove(const Vec2<float>& pos) {
		Event event{};
		event.type = EventType::Move;
		event.x = pos.x;
		event.y = pos.y;
		enqueue(event);
	}

	void pushButton(Button button, bool isDown, const Vec2<float>& pos) {
		Event event{};
		event.type = EventType::ButtonState;
		event.button = (uint8_t)buttonIndex(button);
		event.isDown = isDown ? 1 : 0;
		event.x = pos.x;
		event.y = pos.y;
		enqueue(event);
	}

	void pushWheel(float delta, const Vec2<float>& pos) {
		if (delta == 0.f)
			return;
		Event event{};
		event.type = EventType::Wheel;
		event.wheel = delta;
		event.x = pos.x;
		event.y = pos.y;
		enqueue(event);
	}

	void pushGameAction(int action, int data, const Vec2<float>& pos) {
		switch (action) {
		case 1:
			pushButton(Button::Left, data != 0, pos);
			break;
		case 2:
			pushButton(Button::Right, data != 0, pos);
			break;
		case 3:
			pushButton(Button::Middle, data != 0, pos);
			break;
		case 4:
			pushWheel(data < 0 ? -1.f : 1.f, pos);
			break;
		default:
			pushMove(pos);
			break;
		}
	}

	void divertGameMouseInput() {
		MouseDevice* mouse = MouseDevice::get();
		if (mouse == nullptr)
			return;

		for (const MouseAction& entry : mouse->inputs)
			pushGameAction(entry.action, entry.data, Vec2<float>((float)entry.x, (float)entry.y));
		mouse->inputs.clear();

		mouse->dx = 0;
		mouse->dy = 0;
		mouse->xOld = mouse->x;
		mouse->yOld = mouse->y;

		for (char& state : mouse->buttonStates)
			state = false;
	}

	void beginFrame() {
		Event local[kMaxEvents];
		int count = 0;
		{
			std::lock_guard<std::mutex> lock(g_queueMutex);
			count = g_queueCount;
			if (count > 0)
				memcpy(local, g_queue, sizeof(Event) * (size_t)count);
			g_queueCount = 0;
		}

		for (int i = 0; i < kButtonCount; i++) {
			g_pressed[i] = false;
			g_released[i] = false;
		}
		g_wheel = 0.f;

		bool gotPos = false;
		Vec2<float> newPos = g_pos;
		for (int i = 0; i < count; i++) {
			const Event& event = local[i];
			if (event.type != EventType::ResetButtons) {
				newPos = Vec2<float>(event.x, event.y);
				gotPos = true;
			}

			switch (event.type) {
			case EventType::ButtonState: {
				const int index = event.button < kButtonCount ? event.button : 0;
				if (event.isDown != 0) {
					g_pressed[index] = true;
					g_down[index] = true;
				}
				else {
					g_released[index] = true;
					g_down[index] = false;
				}
				break;
			}
			case EventType::Wheel:
				g_wheel += event.wheel;
				break;
			case EventType::ResetButtons:
				g_wheel = 0.f;
				for (int button = 0; button < kButtonCount; ++button) {
					g_down[button] = false;
					g_pressed[button] = false;
					g_released[button] = false;
					g_lastDeviceButtons[button] = false;
				}
				break;
			case EventType::Move:
			default:
				break;
			}
		}

		pollDevicePosition(gotPos, newPos);
		pollDeviceButtons();

		g_delta = gotPos ? newPos.sub(g_pos) : Vec2<float>(0.f, 0.f);
		if (gotPos)
			g_pos = newPos;

		refreshOwner();
	}

	const Vec2<float>& pos() { return g_pos; }
	const Vec2<float>& delta() { return g_delta; }

	bool isDown(Button button) { return g_down[buttonIndex(button)]; }
	bool pressed(Button button) { return g_pressed[buttonIndex(button)]; }
	bool released(Button button) { return g_released[buttonIndex(button)]; }
	void consume(Button button) { g_pressed[buttonIndex(button)] = false; }

	bool hovered(Vec4<float> rect) {
		return rect.contains(g_pos);
	}

	bool clicked(Vec4<float> rect, Button button) {
		if (!pressed(button) || !rect.contains(g_pos))
			return false;
		consume(button);
		return true;
	}

	float wheel() { return g_wheel; }

	float takeWheel() {
		const float value = g_wheel;
		g_wheel = 0.f;
		return value;
	}

	Owner owner() { return g_owner.load(std::memory_order_relaxed); }
	bool guiOwnsMouse() { return g_guiOwns.load(std::memory_order_acquire); }

	void resetButtons() {
		Event event{};
		event.type = EventType::ResetButtons;
		enqueue(event);
	}

	void refreshOwner() {
		Owner next = Owner::Game;
		if (Client::isInitialized()) {
			static ClickGUI* clickGuiMod = ModuleManager::getModule<ClickGUI>();
			static HudEditor* hudEditorMod = ModuleManager::getModule<HudEditor>();
			if (clickGuiMod != nullptr && clickGuiMod->isEnabled())
				next = Owner::ClickGUI;
			else if (hudEditorMod != nullptr && hudEditorMod->isEnabled())
				next = Owner::HudEditor;
		}

		const Owner current = g_owner.load(std::memory_order_relaxed);
		if (next != current) {
			g_owner.store(next, std::memory_order_relaxed);
			resetButtons();
		}

		g_guiOwns.store(next != Owner::Game, std::memory_order_release);
		g_wantCursorReleased.store(next != Owner::Game, std::memory_order_release);
	}

	void setClientToGuiScale(const Vec2<float>& scale) {
		g_scaleX.store(scale.x > 0.f ? scale.x : 1.f, std::memory_order_relaxed);
		g_scaleY.store(scale.y > 0.f ? scale.y : 1.f, std::memory_order_relaxed);
	}

	Vec2<float> clientToGui(const Vec2<float>& clientPos) {
		return Vec2<float>(clientPos.x * g_scaleX.load(std::memory_order_relaxed),
			clientPos.y * g_scaleY.load(std::memory_order_relaxed));
	}

	void tickCursorArbitration() {
		if (!Client::isInitialized())
			return;

		ClientInstance* clientInstance = Game::clientInstance;
		if (clientInstance == nullptr || clientInstance->minecraftGame == nullptr)
			return;

		std::lock_guard<std::mutex> lock(g_cursorMutex);

		const bool grabbed = clientInstance->minecraftGame->isCursorGrabbed();
		const bool wantReleased = g_wantCursorReleased.load(std::memory_order_acquire);

		if (wantReleased) {
			if (grabbed) {
				g_cursorReleasedByUs.store(true, std::memory_order_release);
				clientInstance->releaseCursor();
			}
			return;
		}

		if (!grabbed && g_cursorReleasedByUs.exchange(false, std::memory_order_acq_rel))
			clientInstance->grabCursor();
	}
}

#pragma once
#include <cstdint>
#include "../../../Utils/Maths.h"

namespace Mouse {

	enum class Button : int {
		Left = 0,
		Right = 1,
		Middle = 2,
		Count = 3
	};

	enum class Owner : int {
		Game = 0,
		ClickGUI = 1,
		HudEditor = 2
	};

	void pushMove(const Vec2<float>& pos);
	void pushButton(Button button, bool isDown, const Vec2<float>& pos);
	void pushWheel(float delta, const Vec2<float>& pos);
	void pushGameAction(int action, int data, const Vec2<float>& pos);

	void divertGameMouseInput();

	void beginFrame();

	const Vec2<float>& pos();
	const Vec2<float>& delta();
	bool isDown(Button button);
	bool pressed(Button button);
	bool released(Button button);
	void consume(Button button);

	bool hovered(Vec4<float> rect);
	bool clicked(Vec4<float> rect, Button button = Button::Left);

	float wheel();
	float takeWheel();

	inline bool leftClicked() { return pressed(Button::Left); }
	inline bool rightClicked() { return pressed(Button::Right); }
	inline bool leftHeld() { return isDown(Button::Left); }
	inline bool rightHeld() { return isDown(Button::Right); }
	inline void consumeLeft() { consume(Button::Left); }
	inline void consumeRight() { consume(Button::Right); }

	Owner owner();
	bool guiOwnsMouse();
	void refreshOwner();

	void setClientToGuiScale(const Vec2<float>& scale);
	Vec2<float> clientToGui(const Vec2<float>& clientPos);

	void tickCursorArbitration();
	void resetButtons();
}

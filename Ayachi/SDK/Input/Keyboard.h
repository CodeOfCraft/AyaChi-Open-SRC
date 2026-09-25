#pragma once
#include <cstdint>

class Keyboard {
public:
	explicit Keyboard(int* gameKeyMap) : keyMap(gameKeyMap) {}

	int* keyMap;
	int keyMapOld[0x100] = {};

	bool isKeyDown(int vKey) const {
		if (keyMap == nullptr || vKey < 0 || vKey >= 0x100)
			return false;
		return keyMap[vKey] != 0;
	}

	template<typename F>
	void poll(F&& onKey) {
		if (keyMap == nullptr)
			return;
		for (int i = 0; i < 0x100; i++) {
			bool isDown = keyMap[i] != 0;
			if (isDown != (keyMapOld[i] != 0))
				onKey(i, isDown);
			keyMapOld[i] = isDown;
		}
	}
};

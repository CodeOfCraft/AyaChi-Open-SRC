#include "ItemPhysics.h"

ItemPhysics::ItemPhysics()
	: Module("ItemPhysics", "Custom physics for dropped items", Category::RENDER) {
	registerSetting(new SliderSetting<float>(
		"Spin Speed", "Airborne item rotation speed", &spinSpeed, spinSpeed, 0.f, 3.f));
	registerSetting(new SliderSetting<float>(
		"Ground Height", "Dropped item height while on the ground", &groundOffset, groundOffset, 0.f, 0.5f));
	registerSetting(new SliderSetting<float>(
		"Air Height", "Dropped item height while airborne", &airOffset, airOffset, 0.f, 0.75f));
}

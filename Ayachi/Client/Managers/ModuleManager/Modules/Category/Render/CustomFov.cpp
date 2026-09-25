#include "CustomFov.h"

CustomFov::CustomFov()
	: Module("CustomFov", "Set the camera field of view without the normal limit", Category::RENDER) {
	registerSetting(new SliderSetting<float>("Fov", "Camera field of view", &fov, 120.f, 1.f, 359.f));
}

void CustomFov::onGetFov(float& currentFov) {
	currentFov = fov;
}

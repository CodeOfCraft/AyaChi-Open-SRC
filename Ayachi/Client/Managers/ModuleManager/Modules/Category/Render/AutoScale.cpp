#include "AutoScale.h"

AutoScale::AutoScale() : Module("AutoScale", "Force a custom GUI scale", Category::RENDER) {
	registerSetting(new SliderSetting<float>("Scale", "GUI scale value", &scale, 2.5f, 0.5f, 5.f));
}

void AutoScale::onTick(Level*) {
	if (Game::clientInstance == nullptr)
		return;

	GuiData* guiData = Game::clientInstance->getGuiData();
	if (guiData == nullptr)
		return;

	guiData->updateGuiScale(scale);
}

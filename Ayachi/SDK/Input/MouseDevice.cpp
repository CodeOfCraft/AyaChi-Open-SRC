#include "MouseDevice.h"
#include "../Runtime/GameData.h"

MouseDevice* MouseDevice::get() {
	return reinterpret_cast<MouseDevice*>(Addresses::MouseDevice_instance);
}

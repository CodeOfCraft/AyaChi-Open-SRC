#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <unordered_map>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <optional>
#include <Psapi.h>
#include <ostream>
#include <sstream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <intrin.h>
#include <memory>
#include <string_view>
#include <winrt/base.h>

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>

#include "Libs/json.hpp"
#include "Libs/xorstr.hpp"
#include "Libs/entt/entt.hpp"
#include "Libs/entt/fwd.hpp"
#include "Libs/glm/glm/glm.hpp"
#include "Libs/kiero/kiero.h"
#include "Libs/libhat/libhat.hpp"
#include "Libs/minhook/MinHook.h"
#include "Libs/imgui/imgui.h"

#include "Utils/ColorUtil.h"
#include "Utils/FileUtil.h"
#include "Utils/Logger.h"
#include "Utils/Maths.h"
#include "Utils/MemoryUtil.h"
#include "Utils/NetworkUtil.h"
#include "Utils/TimerUtil.h"

#include "Client/Client.h"
#include "Client/Managers/NotificationManager/NotificationManager.h"
#include "Client/Managers/FontManager/FontManager.h"
#include "Client/Managers/ModuleManager/Modules/ModuleBase/Settings/BoolSetting.h"
#include "Client/Managers/ModuleManager/Modules/ModuleBase/Settings/ColorSetting.h"
#include "Client/Managers/ModuleManager/Modules/ModuleBase/Settings/EnumSetting.h"
#include "Client/Managers/ModuleManager/Modules/ModuleBase/Settings/KeybindSetting.h"
#include "Client/Managers/ModuleManager/Modules/ModuleBase/Settings/Setting.h"
#include "Client/Managers/ModuleManager/Modules/ModuleBase/Settings/SliderSetting.h"

#endif

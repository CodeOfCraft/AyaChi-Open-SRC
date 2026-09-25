#include "HookManager.h"
#include "Hooks/DirectX/PresentHook.h"
#include "Hooks/DirectX/ResizeBuffersHook.h"
#include "Hooks/DirectX/ResizeBuffers1Hook.h"
#include "../../../Renderer/ImGuiRender.h"
#include "../../../Renderer/D3D12Overlay.h"
#include "../../../SDK/Runtime/GameData.h"
#include "../../../Utils/Logger.h"

#include <kiero.h>

bool HookManager::registerDirectXHooks() {
	int presentIndex = 0;
	int resizeBuffersIndex = 0;
	int resizeBuffers1Index = 0;
	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
		presentIndex = GameData::VTableIndex::DirectX::D3D11::present;
		resizeBuffersIndex = GameData::VTableIndex::DirectX::D3D11::resizeBuffers;
		resizeBuffers1Index = GameData::VTableIndex::DirectX::D3D11::resizeBuffers1;
	}
	else if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success) {
		presentIndex = GameData::VTableIndex::DirectX::D3D12::present;
		resizeBuffersIndex = GameData::VTableIndex::DirectX::D3D12::resizeBuffers;
		resizeBuffers1Index = GameData::VTableIndex::DirectX::D3D12::resizeBuffers1;
	}
	else {
		LOGE("No supported DirectX swap-chain backend was found");
		return false;
	}

	auto** methodsTable = reinterpret_cast<uintptr_t**>(kiero::getMethodsTable());
	RequestHook<PresentHook>(methodsTable, presentIndex);
	RequestHook<ResizeBuffersHook>(methodsTable, resizeBuffersIndex);
	RequestHook<ResizeBuffers1Hook>(methodsTable, resizeBuffers1Index);
	return true;
}

void HookManager::shutdownOverlay() {
	PresentHook::shutdown();
	ImGuiRender::releaseBlur();
	D3D12Overlay::shutdown();
}

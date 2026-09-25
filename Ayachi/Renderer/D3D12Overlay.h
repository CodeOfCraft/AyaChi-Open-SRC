#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>

namespace D3D12Overlay {
	void installCommandQueueHook(ID3D12Device* device);

	bool isReady();

	bool render(IDXGISwapChain3* swapChain, ID3D12Device* device, void (*buildUi)());

	void releaseRenderTargets();

	void shutdown();
}

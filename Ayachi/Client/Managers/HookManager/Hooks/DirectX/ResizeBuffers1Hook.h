#pragma once
#include "../FuncHook.h"
#include "../../../../../Renderer/D3D12Overlay.h"
#include <Windows.h>
#include <dxgi1_4.h>

class ResizeBuffers1Hook : public FuncHook {
private:
	using resizeBuffers1_t = HRESULT(__thiscall*)(IDXGISwapChain3*, UINT, UINT, UINT, DXGI_FORMAT, UINT, const UINT*, IUnknown* const*);
	static inline resizeBuffers1_t oResizeBuffers1;

	static HRESULT resizeBuffers1Callback(IDXGISwapChain3* swapChain, UINT bufferCount, UINT width, UINT height,
		DXGI_FORMAT newFormat, UINT swapChainFlags, const UINT* pCreationNodeMask, IUnknown* const* ppPresentQueue) {
		D3D12Overlay::releaseRenderTargets();
		return oResizeBuffers1(swapChain, bufferCount, width, height, newFormat, swapChainFlags, pCreationNodeMask, ppPresentQueue);
	}
public:
	ResizeBuffers1Hook() {
		OriginFunc = (void*)&oResizeBuffers1;
		func = (void*)&resizeBuffers1Callback;
	}
};

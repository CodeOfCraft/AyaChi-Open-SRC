#pragma once
#include "../FuncHook.h"
#include "../../../../../Renderer/D3D12Overlay.h"
#include <Windows.h>

class ResizeBuffersHook : public FuncHook {
private:
	using reiszeBuffers_t = HRESULT(__thiscall*)(IDXGISwapChain*, int, int, int, DXGI_FORMAT, int);
	static inline reiszeBuffers_t oResizeBuffers;

	static HRESULT resizeBuffersCallback(IDXGISwapChain* swapChain, int bufferCount, int width, int height, DXGI_FORMAT newFormat, int swapChainFlags) {
		D3D12Overlay::releaseRenderTargets();
		return oResizeBuffers(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
	}
public:
	ResizeBuffersHook() {
		OriginFunc = (void*)&oResizeBuffers;
		func = (void*)&resizeBuffersCallback;
	}
};
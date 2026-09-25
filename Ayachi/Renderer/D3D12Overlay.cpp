#include "D3D12Overlay.h"

#include "ImGuiRender.h"
#include "../Utils/Logger.h"
#include "../Libs/minhook/MinHook.h"

#include <imgui.h>
#include <impl/imgui_impl_dx12.h>
#include <impl/imgui_impl_win32.h>

#include <d3dcompiler.h>
#include <algorithm>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

namespace {

	using ExecuteCommandLists_t = void(STDMETHODCALLTYPE*)(ID3D12CommandQueue*, UINT, ID3D12CommandList* const*);
	ExecuteCommandLists_t oExecuteCommandLists = nullptr;
	ID3D12CommandQueue* gameQueue = nullptr;
	bool queueHookInstalled = false;
	bool overlayDisabled = false;
	bool overlayReadyLogged = false;

	constexpr int kExecuteCommandListsIndex = 10;

	void STDMETHODCALLTYPE executeCommandListsDetour(ID3D12CommandQueue* self, UINT numLists, ID3D12CommandList* const* lists) {
		if (gameQueue == nullptr && self != nullptr) {
			const D3D12_COMMAND_QUEUE_DESC desc = self->GetDesc();
			if (desc.Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
				gameQueue = self;
			}
		}
		oExecuteCommandLists(self, numLists, lists);
	}


	struct FrameCtx {
		ID3D12CommandAllocator* allocator = nullptr;
		ID3D12Resource* backBuffer = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE rtv{};
		UINT64 fenceValue = 0;
	};

	bool backendReady = false;
	ID3D12DescriptorHeap* srvHeap = nullptr;
	ID3D12Fence* fence = nullptr;
	HANDLE fenceEvent = nullptr;
	UINT64 fenceCounter = 0;

	bool targetsReady = false;
	ID3D12DescriptorHeap* rtvHeap = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;
	std::vector<FrameCtx> frames;

	constexpr UINT kSrvCount = 64;
	UINT srvStep = 0;
	std::vector<UINT> srvFreeList;

	void srvAlloc(ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* outCpu, D3D12_GPU_DESCRIPTOR_HANDLE* outGpu) {
		if (srvHeap == nullptr || srvFreeList.empty()) {
			*outCpu = {};
			*outGpu = {};
			return;
		}
		const UINT index = srvFreeList.back();
		srvFreeList.pop_back();
		outCpu->ptr = srvHeap->GetCPUDescriptorHandleForHeapStart().ptr + static_cast<SIZE_T>(index) * srvStep;
		outGpu->ptr = srvHeap->GetGPUDescriptorHandleForHeapStart().ptr + static_cast<UINT64>(index) * srvStep;
	}

	void srvFree(ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE) {
		if (srvHeap == nullptr || srvStep == 0)
			return;
		const SIZE_T base = srvHeap->GetCPUDescriptorHandleForHeapStart().ptr;
		if (cpu.ptr < base)
			return;
		srvFreeList.push_back(static_cast<UINT>((cpu.ptr - base) / srvStep));
	}


	constexpr UINT kBlurDownsample = 6;
	constexpr float kBlurRadius = 2.0f;

	ID3D12RootSignature* blurRootSig = nullptr;
	ID3D12PipelineState* copyPso = nullptr;
	ID3D12PipelineState* blurPso = nullptr;

	ID3D12Resource* sceneTex = nullptr;
	ID3D12Resource* pingTex = nullptr;
	ID3D12Resource* pongTex = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE pingRtv{}, pongRtv{};
	D3D12_CPU_DESCRIPTOR_HANDLE sceneSrvCpu{}, pingSrvCpu{}, pongSrvCpu{};
	D3D12_GPU_DESCRIPTOR_HANDLE sceneSrvGpu{}, pingSrvGpu{}, pongSrvGpu{};
	UINT blurWidth = 0, blurHeight = 0;
	bool blurReady = false;
	bool blurSrvSlotsTaken = false;

	const char* kBlurShaderSource = R"(
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_OUTPUT FullscreenVS(uint vertexId : SV_VertexID)
{
	float2 uv = float2((vertexId << 1) & 2, vertexId & 2);
	VS_OUTPUT output;
	output.position = float4(uv.x * 2.0f - 1.0f, 1.0f - uv.y * 2.0f, 0.0f, 1.0f);
	output.uv = uv;
	return output;
}

Texture2D sourceTexture : register(t0);
SamplerState sourceSampler : register(s0);

cbuffer BlurBuffer : register(b0)
{
	float2 texelSize;
	float2 direction;
	float radius;
	float3 padding;
};

float4 CopyPS(VS_OUTPUT input) : SV_Target
{
	float4 color = sourceTexture.Sample(sourceSampler, input.uv);
	color.a = 1.0f;
	return color;
}

float4 BlurPS(VS_OUTPUT input) : SV_Target
{
	float2 stepSize = direction * texelSize * radius;
	float4 color = sourceTexture.Sample(sourceSampler, input.uv) * 0.2270270270f;
	color += sourceTexture.Sample(sourceSampler, input.uv + stepSize * 1.3846153846f) * 0.3162162162f;
	color += sourceTexture.Sample(sourceSampler, input.uv - stepSize * 1.3846153846f) * 0.3162162162f;
	color += sourceTexture.Sample(sourceSampler, input.uv + stepSize * 3.2307692308f) * 0.0702702703f;
	color += sourceTexture.Sample(sourceSampler, input.uv - stepSize * 3.2307692308f) * 0.0702702703f;
	color.a = 1.0f;
	return color;
}
)";

	bool allocSrvSlot(D3D12_CPU_DESCRIPTOR_HANDLE& cpu, D3D12_GPU_DESCRIPTOR_HANDLE& gpu) {
		if (srvHeap == nullptr || srvFreeList.empty())
			return false;
		const UINT index = srvFreeList.back();
		srvFreeList.pop_back();
		cpu.ptr = srvHeap->GetCPUDescriptorHandleForHeapStart().ptr + static_cast<SIZE_T>(index) * srvStep;
		gpu.ptr = srvHeap->GetGPUDescriptorHandleForHeapStart().ptr + static_cast<UINT64>(index) * srvStep;
		return true;
	}

	void transition(ID3D12GraphicsCommandList* cl, ID3D12Resource* res,
		D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
		if (res == nullptr || before == after)
			return;
		D3D12_RESOURCE_BARRIER b{};
		b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		b.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		b.Transition.pResource = res;
		b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		b.Transition.StateBefore = before;
		b.Transition.StateAfter = after;
		cl->ResourceBarrier(1, &b);
	}

	ID3D12Resource* createTexture2D(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initialState) {
		D3D12_HEAP_PROPERTIES heap{};
		heap.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap.CreationNodeMask = 1;
		heap.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = flags;

		D3D12_CLEAR_VALUE clear{};
		clear.Format = format;
		const bool isRenderTarget = (flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0;

		ID3D12Resource* resource = nullptr;
		if (FAILED(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, initialState,
			isRenderTarget ? &clear : nullptr, IID_PPV_ARGS(&resource))))
			return nullptr;
		return resource;
	}

	bool compileBlurShaders(ID3D12Device* device, DXGI_FORMAT rtvFormat) {
		D3D12_DESCRIPTOR_RANGE srvRange{};
		srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRange.NumDescriptors = 1;
		srvRange.BaseShaderRegister = 0;
		srvRange.RegisterSpace = 0;
		srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_PARAMETER params[2]{};
		params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		params[0].DescriptorTable.NumDescriptorRanges = 1;
		params[0].DescriptorTable.pDescriptorRanges = &srvRange;
		params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		params[1].Constants.ShaderRegister = 0;
		params[1].Constants.RegisterSpace = 0;
		params[1].Constants.Num32BitValues = 8;
		params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_STATIC_SAMPLER_DESC sampler{};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_ROOT_SIGNATURE_DESC rootDesc{};
		rootDesc.NumParameters = 2;
		rootDesc.pParameters = params;
		rootDesc.NumStaticSamplers = 1;
		rootDesc.pStaticSamplers = &sampler;
		rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		using SerializeRootSignature_t = HRESULT(WINAPI*)(const D3D12_ROOT_SIGNATURE_DESC*, D3D_ROOT_SIGNATURE_VERSION, ID3DBlob**, ID3DBlob**);
		HMODULE d3d12Module = GetModuleHandleA("d3d12.dll");
		auto serializeRootSignature = d3d12Module
			? reinterpret_cast<SerializeRootSignature_t>(GetProcAddress(d3d12Module, "D3D12SerializeRootSignature"))
			: nullptr;
		if (serializeRootSignature == nullptr) {
			LOGW("D3D12SerializeRootSignature is unavailable; blur is disabled");
			return false;
		}

		ID3DBlob* rootBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		if (FAILED(serializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootBlob, &errorBlob))) {
			LOGW("D3D12 blur root-signature serialization failed; blur is disabled");
			if (rootBlob) rootBlob->Release();
			if (errorBlob) errorBlob->Release();
			return false;
		}
		if (errorBlob) { errorBlob->Release(); errorBlob = nullptr; }

		const HRESULT rootResult = device->CreateRootSignature(0, rootBlob->GetBufferPointer(),
			rootBlob->GetBufferSize(), IID_PPV_ARGS(&blurRootSig));
		rootBlob->Release();
		if (FAILED(rootResult)) {
			LOGW("D3D12 blur root-signature creation failed; blur is disabled");
			return false;
		}

		ID3DBlob* vsBlob = nullptr;
		ID3DBlob* copyBlob = nullptr;
		ID3DBlob* blurBlob = nullptr;
		const UINT compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
		if (FAILED(D3DCompile(kBlurShaderSource, strlen(kBlurShaderSource), nullptr, nullptr, nullptr,
			"FullscreenVS", "vs_5_0", compileFlags, 0, &vsBlob, nullptr)) ||
			FAILED(D3DCompile(kBlurShaderSource, strlen(kBlurShaderSource), nullptr, nullptr, nullptr,
				"CopyPS", "ps_5_0", compileFlags, 0, &copyBlob, nullptr)) ||
			FAILED(D3DCompile(kBlurShaderSource, strlen(kBlurShaderSource), nullptr, nullptr, nullptr,
				"BlurPS", "ps_5_0", compileFlags, 0, &blurBlob, nullptr))) {
			LOGW("D3D12 blur shader compilation failed; blur is disabled");
			if (vsBlob) vsBlob->Release();
			if (copyBlob) copyBlob->Release();
			if (blurBlob) blurBlob->Release();
			return false;
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		psoDesc.pRootSignature = blurRootSig;
		psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = rtvFormat;
		psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		psoDesc.SampleDesc.Count = 1;

		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		psoDesc.RasterizerState.DepthClipEnable = TRUE;

		psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		psoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;

		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;

		psoDesc.PS = { copyBlob->GetBufferPointer(), copyBlob->GetBufferSize() };
		const HRESULT copyResult = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&copyPso));

		psoDesc.PS = { blurBlob->GetBufferPointer(), blurBlob->GetBufferSize() };
		const HRESULT blurResult = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&blurPso));

		vsBlob->Release();
		copyBlob->Release();
		blurBlob->Release();

		if (FAILED(copyResult) || FAILED(blurResult)) {
			LOGW("D3D12 blur pipeline creation failed; blur is disabled");
			return false;
		}
		return true;
	}

	void destroyBlur() {
		blurReady = false;
		if (sceneTex) { sceneTex->Release(); sceneTex = nullptr; }
		if (pingTex) { pingTex->Release(); pingTex = nullptr; }
		if (pongTex) { pongTex->Release(); pongTex = nullptr; }
		blurWidth = 0;
		blurHeight = 0;
		ImGuiRender::setExternalBlurTexture(0);
	}

	void destroyBlurShaders() {
		if (copyPso) { copyPso->Release(); copyPso = nullptr; }
		if (blurPso) { blurPso->Release(); blurPso = nullptr; }
		if (blurRootSig) { blurRootSig->Release(); blurRootSig = nullptr; }
	}

	bool createBlur(ID3D12Device* device, const DXGI_SWAP_CHAIN_DESC& desc,
		D3D12_CPU_DESCRIPTOR_HANDLE rtvBase, UINT rtvStep, UINT rtvSlot) {
		const DXGI_FORMAT format = desc.BufferDesc.Format;
		blurWidth = (std::max)(1u, (desc.BufferDesc.Width + kBlurDownsample - 1u) / kBlurDownsample);
		blurHeight = (std::max)(1u, (desc.BufferDesc.Height + kBlurDownsample - 1u) / kBlurDownsample);

		if (blurRootSig == nullptr && !compileBlurShaders(device, format))
			return false;

		sceneTex = createTexture2D(device, desc.BufferDesc.Width, desc.BufferDesc.Height, format,
			D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COPY_DEST);
		pingTex = createTexture2D(device, blurWidth, blurHeight, format,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		pongTex = createTexture2D(device, blurWidth, blurHeight, format,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		if (sceneTex == nullptr || pingTex == nullptr || pongTex == nullptr) {
			LOGW("D3D12 blur texture creation failed; blur is disabled");
			return false;
		}

		pingRtv.ptr = rtvBase.ptr + static_cast<SIZE_T>(rtvSlot) * rtvStep;
		pongRtv.ptr = rtvBase.ptr + static_cast<SIZE_T>(rtvSlot + 1) * rtvStep;
		device->CreateRenderTargetView(pingTex, nullptr, pingRtv);
		device->CreateRenderTargetView(pongTex, nullptr, pongRtv);

		if (!blurSrvSlotsTaken) {
			if (!allocSrvSlot(sceneSrvCpu, sceneSrvGpu) ||
				!allocSrvSlot(pingSrvCpu, pingSrvGpu) ||
				!allocSrvSlot(pongSrvCpu, pongSrvGpu)) {
				LOGW("D3D12 blur descriptor allocation failed; blur is disabled");
				return false;
			}
			blurSrvSlotsTaken = true;
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(sceneTex, &srvDesc, sceneSrvCpu);
		device->CreateShaderResourceView(pingTex, &srvDesc, pingSrvCpu);
		device->CreateShaderResourceView(pongTex, &srvDesc, pongSrvCpu);

		blurReady = true;
		return true;
	}

	void fullscreenPass(ID3D12GraphicsCommandList* cl, ID3D12PipelineState* pso,
		D3D12_GPU_DESCRIPTOR_HANDLE srcSrv, D3D12_CPU_DESCRIPTOR_HANDLE dstRtv,
		float dirX, float dirY) {
		cl->SetPipelineState(pso);
		cl->SetGraphicsRootSignature(blurRootSig);
		cl->SetGraphicsRootDescriptorTable(0, srcSrv);

		const float constants[8] = {
			1.f / static_cast<float>(blurWidth), 1.f / static_cast<float>(blurHeight),
			dirX, dirY, kBlurRadius, 0.f, 0.f, 0.f
		};
		cl->SetGraphicsRoot32BitConstants(1, 8, constants, 0);

		D3D12_VIEWPORT viewport{ 0.f, 0.f, static_cast<float>(blurWidth), static_cast<float>(blurHeight), 0.f, 1.f };
		D3D12_RECT scissor{ 0, 0, static_cast<LONG>(blurWidth), static_cast<LONG>(blurHeight) };
		cl->RSSetViewports(1, &viewport);
		cl->RSSetScissorRects(1, &scissor);
		cl->OMSetRenderTargets(1, &dstRtv, FALSE, nullptr);
		cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cl->DrawInstanced(3, 1, 0, 0);
	}

	void recordBlur(ID3D12GraphicsCommandList* cl, ID3D12Resource* backBuffer) {
		if (!blurReady || backBuffer == nullptr)
			return;

		transition(cl, backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE);
		cl->CopyResource(sceneTex, backBuffer);
		transition(cl, backBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT);
		transition(cl, sceneTex, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		transition(cl, pingTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		fullscreenPass(cl, copyPso, sceneSrvGpu, pingRtv, 0.f, 0.f);

		transition(cl, pingTex, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		transition(cl, pongTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		fullscreenPass(cl, blurPso, pingSrvGpu, pongRtv, 1.f, 0.f);

		transition(cl, pongTex, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		transition(cl, pingTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		fullscreenPass(cl, blurPso, pongSrvGpu, pingRtv, 0.f, 1.f);

		transition(cl, pingTex, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		transition(cl, sceneTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	}

	void waitForGpuIdle() {
		if (fence == nullptr || fenceEvent == nullptr || gameQueue == nullptr)
			return;

		const UINT64 target = ++fenceCounter;
		if (FAILED(gameQueue->Signal(fence, target)))
			return;
		if (fence->GetCompletedValue() >= target)
			return;
		if (SUCCEEDED(fence->SetEventOnCompletion(target, fenceEvent)))
			WaitForSingleObject(fenceEvent, 1000);
	}

	void destroyTargets() {
		if (targetsReady)
			waitForGpuIdle();

		destroyBlur();

		for (FrameCtx& f : frames) {
			if (f.backBuffer) { f.backBuffer->Release(); f.backBuffer = nullptr; }
			if (f.allocator) { f.allocator->Release(); f.allocator = nullptr; }
		}
		frames.clear();
		if (cmdList) { cmdList->Release(); cmdList = nullptr; }
		if (rtvHeap) { rtvHeap->Release(); rtvHeap = nullptr; }
		targetsReady = false;
	}

	bool createBackend(IDXGISwapChain3* swapChain, ID3D12Device* device, const DXGI_SWAP_CHAIN_DESC& desc) {
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.NumDescriptors = kSrvCount;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)))) {
			LOGE("D3D12 SRV heap creation failed");
			return false;
		}
		srvStep = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		srvFreeList.clear();
		srvFreeList.reserve(kSrvCount);
		for (int i = static_cast<int>(kSrvCount) - 1; i >= 0; i--)
			srvFreeList.push_back(static_cast<UINT>(i));

		if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))) {
			LOGE("D3D12 fence creation failed");
			return false;
		}
		fenceEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr);
		if (fenceEvent == nullptr) {
			LOGE("D3D12 fence event creation failed (error=%lu)", GetLastError());
			return false;
		}

		ImGui_ImplDX12_InitInfo info{};
		info.Device = device;
		info.CommandQueue = gameQueue;
		info.NumFramesInFlight = static_cast<int>(desc.BufferCount);
		info.RTVFormat = desc.BufferDesc.Format;
		info.DSVFormat = DXGI_FORMAT_UNKNOWN;
		info.SrvDescriptorHeap = srvHeap;
		info.SrvDescriptorAllocFn = &srvAlloc;
		info.SrvDescriptorFreeFn = &srvFree;
		if (!ImGui_ImplDX12_Init(&info)) {
			LOGE("ImGui D3D12 backend initialization failed");
			return false;
		}
		return true;
	}

	bool createTargets(IDXGISwapChain3* swapChain, ID3D12Device* device, const DXGI_SWAP_CHAIN_DESC& desc) {
		const UINT bufferCount = desc.BufferCount;

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = bufferCount + 2;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)))) {
			LOGE("D3D12 RTV heap creation failed");
			return false;
		}

		const UINT rtvStep = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

		frames.assign(bufferCount, FrameCtx{});
		for (UINT i = 0; i < bufferCount; i++) {
			if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frames[i].allocator)))) {
				LOGE("D3D12 command allocator creation failed (buffer=%u)", i);
				return false;
			}
			if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&frames[i].backBuffer)))) {
				LOGE("D3D12 back-buffer acquisition failed (buffer=%u)", i);
				return false;
			}
			device->CreateRenderTargetView(frames[i].backBuffer, nullptr, rtvHandle);
			frames[i].rtv = rtvHandle;
			rtvHandle.ptr += rtvStep;
		}

		if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frames[0].allocator, nullptr, IID_PPV_ARGS(&cmdList)))) {
			LOGE("D3D12 command-list creation failed");
			return false;
		}
		cmdList->Close();

		if (!createBlur(device, desc, rtvHeap->GetCPUDescriptorHandleForHeapStart(), rtvStep, bufferCount)) {
			destroyBlur();
		}
		if (!overlayReadyLogged) {
			overlayReadyLogged = true;
			LOGI("Overlay initialized (renderer=D3D12, size=%ux%u, format=%d, buffers=%u, blur=%s)",
				desc.BufferDesc.Width, desc.BufferDesc.Height, static_cast<int>(desc.BufferDesc.Format),
				bufferCount, blurReady ? "enabled" : "disabled");
		}
		return true;
	}
}

void D3D12Overlay::installCommandQueueHook(ID3D12Device* device) {
	if (queueHookInstalled || device == nullptr)
		return;
	queueHookInstalled = true;

	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	ID3D12CommandQueue* probe = nullptr;
	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&probe))) || probe == nullptr) {
		LOGE("D3D12 command-queue probe creation failed; the overlay is disabled");
		return;
	}

	void** vtable = *reinterpret_cast<void***>(probe);
	void* target = vtable[kExecuteCommandListsIndex];
	probe->Release();

	const MH_STATUS createStatus = MH_CreateHook(target, reinterpret_cast<void*>(&executeCommandListsDetour),
		reinterpret_cast<void**>(&oExecuteCommandLists));
	if (createStatus != MH_OK) {
		LOGE("Failed to create the D3D12 ExecuteCommandLists hook: %s",
			MH_StatusToString(createStatus));
		return;
	}
	const MH_STATUS enableStatus = MH_EnableHook(target);
	if (enableStatus != MH_OK) {
		LOGE("Failed to enable the D3D12 ExecuteCommandLists hook: %s",
			MH_StatusToString(enableStatus));
		return;
	}
}

bool D3D12Overlay::isReady() {
	return gameQueue != nullptr && !overlayDisabled;
}

bool D3D12Overlay::render(IDXGISwapChain3* swapChain, ID3D12Device* device, void (*buildUi)()) {
	if (overlayDisabled || swapChain == nullptr || device == nullptr || gameQueue == nullptr || buildUi == nullptr)
		return false;

	DXGI_SWAP_CHAIN_DESC desc{};
	if (FAILED(swapChain->GetDesc(&desc)) || desc.BufferCount == 0)
		return false;

	if (!backendReady) {
		if (!createBackend(swapChain, device, desc)) {
			overlayDisabled = true;
			shutdown();
			return false;
		}
		backendReady = true;
	}

	if (!targetsReady) {
		if (!createTargets(swapChain, device, desc)) {
			destroyTargets();
			return false;
		}
		targetsReady = true;
	}

	const UINT index = swapChain->GetCurrentBackBufferIndex();
	if (index >= frames.size())
		return false;
	FrameCtx& frame = frames[index];

	if (frame.fenceValue != 0 && fence->GetCompletedValue() < frame.fenceValue) {
		if (SUCCEEDED(fence->SetEventOnCompletion(frame.fenceValue, fenceEvent)))
			WaitForSingleObject(fenceEvent, 1000);
	}

	frame.allocator->Reset();
	cmdList->Reset(frame.allocator, nullptr);
	cmdList->SetDescriptorHeaps(1, &srvHeap);

	recordBlur(cmdList, frame.backBuffer);
	ImGuiRender::setExternalBlurTexture(
		blurReady ? static_cast<ImTextureID>(pingSrvGpu.ptr) : static_cast<ImTextureID>(0));

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();

	if (desc.BufferDesc.Width > 0 && desc.BufferDesc.Height > 0) {
		ImGui::GetIO().DisplaySize = ImVec2(
			static_cast<float>(desc.BufferDesc.Width),
			static_cast<float>(desc.BufferDesc.Height));
	}

	ImGui::NewFrame();

	buildUi();

	ImGui::EndFrame();
	ImGui::Render();

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = frame.backBuffer;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	cmdList->ResourceBarrier(1, &barrier);

	D3D12_VIEWPORT viewport{ 0.f, 0.f,
		static_cast<float>(desc.BufferDesc.Width), static_cast<float>(desc.BufferDesc.Height), 0.f, 1.f };
	D3D12_RECT scissor{ 0, 0,
		static_cast<LONG>(desc.BufferDesc.Width), static_cast<LONG>(desc.BufferDesc.Height) };
	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &scissor);

	cmdList->OMSetRenderTargets(1, &frame.rtv, FALSE, nullptr);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	cmdList->ResourceBarrier(1, &barrier);
	cmdList->Close();

	ID3D12CommandList* lists[] = { cmdList };
	oExecuteCommandLists(gameQueue, 1, lists);

	frame.fenceValue = ++fenceCounter;
	gameQueue->Signal(fence, frame.fenceValue);
	return true;
}

void D3D12Overlay::releaseRenderTargets() {
	destroyTargets();
}

void D3D12Overlay::shutdown() {
	destroyTargets();

	if (backendReady) {
		waitForGpuIdle();
		ImGui_ImplDX12_Shutdown();
		backendReady = false;
	}

	destroyBlurShaders();

	if (srvHeap) { srvHeap->Release(); srvHeap = nullptr; }
	if (fence) { fence->Release(); fence = nullptr; }
	if (fenceEvent) { CloseHandle(fenceEvent); fenceEvent = nullptr; }

	srvFreeList.clear();
	srvStep = 0;
	fenceCounter = 0;
	blurSrvSlotsTaken = false;
}

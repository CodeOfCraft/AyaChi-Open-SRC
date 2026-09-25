#include "../pch.h"
#include "ImGuiRender.h"
#include "../Client/Client.h"
#include "../SDK/Core/Client/MinecraftGame.h"
#include "../SDK/Render/GameRenderer.h"
#include "../SDK/Render/LevelRenderer.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace {
	float cross(const ImVec2& origin, const ImVec2& a, const ImVec2& b) {
		return (a.x - origin.x) * (b.y - origin.y) -
			(a.y - origin.y) * (b.x - origin.x);
	}

	std::vector<ImVec2> buildConvexHull(std::array<ImVec2, 8> points) {
		std::sort(points.begin(), points.end(), [](const ImVec2& lhs, const ImVec2& rhs) {
			return lhs.x == rhs.x ? lhs.y < rhs.y : lhs.x < rhs.x;
		});

		std::vector<ImVec2> hull;
		hull.reserve(points.size() * 2);
		for (const ImVec2& point : points) {
			while (hull.size() >= 2 && cross(hull[hull.size() - 2], hull.back(), point) <= 0.f)
				hull.pop_back();
			hull.push_back(point);
		}

		const std::size_t lowerSize = hull.size();
		for (std::size_t index = points.size() - 1; index-- > 0;) {
			const ImVec2& point = points[index];
			while (hull.size() > lowerSize && cross(hull[hull.size() - 2], hull.back(), point) <= 0.f)
				hull.pop_back();
			hull.push_back(point);
		}

		if (!hull.empty())
			hull.pop_back();
		return hull;
	}

	constexpr UINT BlurDownsample = 6;
	constexpr int BlurPassCount = 1;
	constexpr UINT64 BlurRefreshInterval = 2;
	constexpr float BlurRadius = 2.0f;

	template <typename T>
	void safeRelease(T*& resource) {
		if (resource) {
			resource->Release();
			resource = nullptr;
		}
	}

	struct BlurConstants {
		float texelSize[2];
		float direction[2];
		float radius;
		float padding[3];
	};

	struct D3D11StateBackup {
		ID3D11RenderTargetView* renderTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
		ID3D11DepthStencilView* depthStencilView = nullptr;
		ID3D11RasterizerState* rasterizerState = nullptr;
		ID3D11BlendState* blendState = nullptr;
		ID3D11DepthStencilState* depthStencilState = nullptr;
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3D11GeometryShader* geometryShader = nullptr;
		ID3D11HullShader* hullShader = nullptr;
		ID3D11DomainShader* domainShader = nullptr;
		ID3D11ComputeShader* computeShader = nullptr;
		ID3D11ShaderResourceView* pixelShaderResource = nullptr;
		ID3D11SamplerState* pixelSampler = nullptr;
		ID3D11Buffer* pixelConstantBuffer = nullptr;
		ID3D11InputLayout* inputLayout = nullptr;
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		D3D11_VIEWPORT viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
		D3D11_RECT scissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
		FLOAT blendFactor[4] = {};
		UINT sampleMask = 0xffffffff;
		UINT stencilRef = 0;
		UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		UINT scissorRectCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		UINT vertexBufferStride = 0;
		UINT vertexBufferOffset = 0;
		UINT indexBufferOffset = 0;
		DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_UNKNOWN;
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

		void capture(ID3D11DeviceContext* context) {
			context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, renderTargets, &depthStencilView);
			context->RSGetState(&rasterizerState);
			context->RSGetViewports(&viewportCount, viewports);
			context->RSGetScissorRects(&scissorRectCount, scissorRects);
			context->OMGetBlendState(&blendState, blendFactor, &sampleMask);
			context->OMGetDepthStencilState(&depthStencilState, &stencilRef);
			context->VSGetShader(&vertexShader, nullptr, nullptr);
			context->PSGetShader(&pixelShader, nullptr, nullptr);
			context->GSGetShader(&geometryShader, nullptr, nullptr);
			context->HSGetShader(&hullShader, nullptr, nullptr);
			context->DSGetShader(&domainShader, nullptr, nullptr);
			context->CSGetShader(&computeShader, nullptr, nullptr);
			context->PSGetShaderResources(0, 1, &pixelShaderResource);
			context->PSGetSamplers(0, 1, &pixelSampler);
			context->PSGetConstantBuffers(0, 1, &pixelConstantBuffer);
			context->IAGetInputLayout(&inputLayout);
			context->IAGetPrimitiveTopology(&topology);
			context->IAGetVertexBuffers(0, 1, &vertexBuffer, &vertexBufferStride, &vertexBufferOffset);
			context->IAGetIndexBuffer(&indexBuffer, &indexBufferFormat, &indexBufferOffset);
		}

		void restore(ID3D11DeviceContext* context) {
			context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, renderTargets, depthStencilView);
			context->RSSetState(rasterizerState);
			context->RSSetViewports(viewportCount, viewports);
			context->RSSetScissorRects(scissorRectCount, scissorRects);
			context->OMSetBlendState(blendState, blendFactor, sampleMask);
			context->OMSetDepthStencilState(depthStencilState, stencilRef);
			context->VSSetShader(vertexShader, nullptr, 0);
			context->PSSetShader(pixelShader, nullptr, 0);
			context->GSSetShader(geometryShader, nullptr, 0);
			context->HSSetShader(hullShader, nullptr, 0);
			context->DSSetShader(domainShader, nullptr, 0);
			context->CSSetShader(computeShader, nullptr, 0);
			context->PSSetShaderResources(0, 1, &pixelShaderResource);
			context->PSSetSamplers(0, 1, &pixelSampler);
			context->PSSetConstantBuffers(0, 1, &pixelConstantBuffer);
			context->IASetInputLayout(inputLayout);
			context->IASetPrimitiveTopology(topology);
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexBufferStride, &vertexBufferOffset);
			context->IASetIndexBuffer(indexBuffer, indexBufferFormat, indexBufferOffset);

			for (auto& renderTarget : renderTargets)
				safeRelease(renderTarget);
			safeRelease(depthStencilView);
			safeRelease(rasterizerState);
			safeRelease(blendState);
			safeRelease(depthStencilState);
			safeRelease(vertexShader);
			safeRelease(pixelShader);
			safeRelease(geometryShader);
			safeRelease(hullShader);
			safeRelease(domainShader);
			safeRelease(computeShader);
			safeRelease(pixelShaderResource);
			safeRelease(pixelSampler);
			safeRelease(pixelConstantBuffer);
			safeRelease(inputLayout);
			safeRelease(vertexBuffer);
			safeRelease(indexBuffer);
		}
	};

	struct BlurPipeline {
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		ID3D11Texture2D* backBuffer = nullptr;

		ID3D11Texture2D* sceneTexture = nullptr;
		ID3D11ShaderResourceView* sceneSrv = nullptr;
		ID3D11Texture2D* pingTexture = nullptr;
		ID3D11ShaderResourceView* pingSrv = nullptr;
		ID3D11RenderTargetView* pingRtv = nullptr;
		ID3D11Texture2D* pongTexture = nullptr;
		ID3D11ShaderResourceView* pongSrv = nullptr;
		ID3D11RenderTargetView* pongRtv = nullptr;
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* copyShader = nullptr;
		ID3D11PixelShader* blurShader = nullptr;
		ID3D11Buffer* constantsBuffer = nullptr;
		ID3D11SamplerState* samplerState = nullptr;
		ID3D11BlendState* blendState = nullptr;
		ID3D11DepthStencilState* depthStencilState = nullptr;
		ID3D11RasterizerState* rasterizerState = nullptr;

		UINT sourceWidth = 0;
		UINT sourceHeight = 0;
		UINT targetWidth = 0;
		UINT targetHeight = 0;
		DXGI_FORMAT sourceFormat = DXGI_FORMAT_UNKNOWN;
		UINT64 frameIndex = 0;
		UINT64 lastPreparedFrame = 0;
		bool hasPreparedFrame = false;
		bool frameReady = false;

		void beginFrame(ID3D11Device* newDevice, ID3D11DeviceContext* newContext, ID3D11Texture2D* newBackBuffer) {
			if (device != newDevice) {
				releaseAll();
				device = newDevice;
			}

			frameIndex++;
			context = newContext;
			backBuffer = newBackBuffer;
			frameReady = false;
		}

		void endFrame() {
			context = nullptr;
			backBuffer = nullptr;
			frameReady = false;
		}

		void releaseAll() {
			releaseTargets();
			safeRelease(vertexShader);
			safeRelease(copyShader);
			safeRelease(blurShader);
			safeRelease(constantsBuffer);
			safeRelease(samplerState);
			safeRelease(blendState);
			safeRelease(depthStencilState);
			safeRelease(rasterizerState);
			device = nullptr;
			context = nullptr;
			backBuffer = nullptr;
			frameReady = false;
		}

		ID3D11ShaderResourceView* getSrv() const {
			return pingSrv;
		}

		bool prepare() {
			if (frameReady)
				return true;
			if (!device || !context || !backBuffer)
				return false;

			D3D11_TEXTURE2D_DESC backBufferDesc = {};
			backBuffer->GetDesc(&backBufferDesc);
			if (backBufferDesc.Width == 0 || backBufferDesc.Height == 0 || backBufferDesc.Format == DXGI_FORMAT_UNKNOWN)
				return false;
			if (backBufferDesc.SampleDesc.Count != 1)
				return false;

			const UINT nextTargetWidth = getBlurTargetSize(backBufferDesc.Width);
			const UINT nextTargetHeight = getBlurTargetSize(backBufferDesc.Height);
			const bool canReuseCachedFrame =
				hasPreparedFrame && pingSrv &&
				sourceWidth == backBufferDesc.Width && sourceHeight == backBufferDesc.Height &&
				targetWidth == nextTargetWidth && targetHeight == nextTargetHeight &&
				sourceFormat == backBufferDesc.Format &&
				frameIndex > lastPreparedFrame && frameIndex - lastPreparedFrame < BlurRefreshInterval;
			if (canReuseCachedFrame) {
				frameReady = true;
				return true;
			}

			if (!ensureShaders() || !ensureTargets(backBufferDesc))
				return false;

			D3D11StateBackup backup;
			backup.capture(context);

			context->OMSetRenderTargets(0, nullptr, nullptr);
			context->CopyResource(sceneTexture, backBuffer);

			renderFullscreen(sceneSrv, pingRtv, copyShader);
			for (int i = 0; i < BlurPassCount; i++) {
				updateConstants(1.f, 0.f);
				renderFullscreen(pingSrv, pongRtv, blurShader);
				updateConstants(0.f, 1.f);
				renderFullscreen(pongSrv, pingRtv, blurShader);
			}

			backup.restore(context);

			lastPreparedFrame = frameIndex;
			hasPreparedFrame = true;
			frameReady = true;
			return true;
		}

	private:
		static UINT getBlurTargetSize(UINT sourceSize) {
			return std::max(1u, (sourceSize + BlurDownsample - 1u) / BlurDownsample);
		}

		void releaseTargets() {
			safeRelease(sceneTexture);
			safeRelease(sceneSrv);
			safeRelease(pingTexture);
			safeRelease(pingSrv);
			safeRelease(pingRtv);
			safeRelease(pongTexture);
			safeRelease(pongSrv);
			safeRelease(pongRtv);
			sourceWidth = 0;
			sourceHeight = 0;
			targetWidth = 0;
			targetHeight = 0;
			sourceFormat = DXGI_FORMAT_UNKNOWN;
			hasPreparedFrame = false;
			lastPreparedFrame = 0;
			frameReady = false;
		}

		bool ensureShaders() {
			if (vertexShader && copyShader && blurShader && constantsBuffer && samplerState && blendState && depthStencilState && rasterizerState)
				return true;

			static const char* shaderSource = R"(
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

			ID3DBlob* vertexBlob = nullptr;
			ID3DBlob* copyBlob = nullptr;
			ID3DBlob* blurBlob = nullptr;
			ID3DBlob* errorBlob = nullptr;

			if (FAILED(D3DCompile(shaderSource, strlen(shaderSource), nullptr, nullptr, nullptr, "FullscreenVS", "vs_4_0", 0, 0, &vertexBlob, &errorBlob))) {
				safeRelease(errorBlob);
				return false;
			}
			if (FAILED(D3DCompile(shaderSource, strlen(shaderSource), nullptr, nullptr, nullptr, "CopyPS", "ps_4_0", 0, 0, &copyBlob, &errorBlob))) {
				safeRelease(vertexBlob);
				safeRelease(errorBlob);
				return false;
			}
			if (FAILED(D3DCompile(shaderSource, strlen(shaderSource), nullptr, nullptr, nullptr, "BlurPS", "ps_4_0", 0, 0, &blurBlob, &errorBlob))) {
				safeRelease(vertexBlob);
				safeRelease(copyBlob);
				safeRelease(errorBlob);
				return false;
			}

			const bool createdShaders =
				SUCCEEDED(device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &vertexShader)) &&
				SUCCEEDED(device->CreatePixelShader(copyBlob->GetBufferPointer(), copyBlob->GetBufferSize(), nullptr, &copyShader)) &&
				SUCCEEDED(device->CreatePixelShader(blurBlob->GetBufferPointer(), blurBlob->GetBufferSize(), nullptr, &blurShader));

			safeRelease(vertexBlob);
			safeRelease(copyBlob);
			safeRelease(blurBlob);
			safeRelease(errorBlob);

			if (!createdShaders) {
				releaseAll();
				return false;
			}

			D3D11_BUFFER_DESC constantsDesc = {};
			constantsDesc.ByteWidth = sizeof(BlurConstants);
			constantsDesc.Usage = D3D11_USAGE_DEFAULT;
			constantsDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			if (FAILED(device->CreateBuffer(&constantsDesc, nullptr, &constantsBuffer))) {
				releaseAll();
				return false;
			}

			D3D11_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.MinLOD = 0.f;
			samplerDesc.MaxLOD = 0.f;
			if (FAILED(device->CreateSamplerState(&samplerDesc, &samplerState))) {
				releaseAll();
				return false;
			}

			D3D11_BLEND_DESC blendDesc = {};
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			if (FAILED(device->CreateBlendState(&blendDesc, &blendState))) {
				releaseAll();
				return false;
			}

			D3D11_DEPTH_STENCIL_DESC depthDesc = {};
			depthDesc.DepthEnable = false;
			depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			if (FAILED(device->CreateDepthStencilState(&depthDesc, &depthStencilState))) {
				releaseAll();
				return false;
			}

			D3D11_RASTERIZER_DESC rasterizerDesc = {};
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			rasterizerDesc.ScissorEnable = false;
			rasterizerDesc.DepthClipEnable = true;
			if (FAILED(device->CreateRasterizerState(&rasterizerDesc, &rasterizerState))) {
				releaseAll();
				return false;
			}

			return true;
		}

		bool ensureTargets(const D3D11_TEXTURE2D_DESC& backBufferDesc) {
			const UINT nextTargetWidth = getBlurTargetSize(backBufferDesc.Width);
			const UINT nextTargetHeight = getBlurTargetSize(backBufferDesc.Height);
			if (sceneTexture && sceneSrv && pingTexture && pingSrv && pingRtv && pongTexture && pongSrv && pongRtv &&
				sourceWidth == backBufferDesc.Width && sourceHeight == backBufferDesc.Height &&
				targetWidth == nextTargetWidth && targetHeight == nextTargetHeight && sourceFormat == backBufferDesc.Format) {
				return true;
			}

			releaseTargets();
			sourceWidth = backBufferDesc.Width;
			sourceHeight = backBufferDesc.Height;
			targetWidth = nextTargetWidth;
			targetHeight = nextTargetHeight;
			sourceFormat = backBufferDesc.Format;

			D3D11_TEXTURE2D_DESC sceneDesc = {};
			sceneDesc.Width = sourceWidth;
			sceneDesc.Height = sourceHeight;
			sceneDesc.MipLevels = 1;
			sceneDesc.ArraySize = 1;
			sceneDesc.Format = sourceFormat;
			sceneDesc.SampleDesc.Count = 1;
			sceneDesc.Usage = D3D11_USAGE_DEFAULT;
			sceneDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			if (FAILED(device->CreateTexture2D(&sceneDesc, nullptr, &sceneTexture))) {
				releaseTargets();
				return false;
			}
			if (FAILED(device->CreateShaderResourceView(sceneTexture, nullptr, &sceneSrv))) {
				releaseTargets();
				return false;
			}

			D3D11_TEXTURE2D_DESC blurDesc = sceneDesc;
			blurDesc.Width = targetWidth;
			blurDesc.Height = targetHeight;
			blurDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

			if (!createBlurTarget(blurDesc, pingTexture, pingSrv, pingRtv)) {
				releaseTargets();
				return false;
			}
			if (!createBlurTarget(blurDesc, pongTexture, pongSrv, pongRtv)) {
				releaseTargets();
				return false;
			}

			return true;
		}

		bool createBlurTarget(const D3D11_TEXTURE2D_DESC& desc, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv, ID3D11RenderTargetView*& rtv) {
			if (FAILED(device->CreateTexture2D(&desc, nullptr, &texture))) {
				safeRelease(texture);
				return false;
			}
			if (FAILED(device->CreateShaderResourceView(texture, nullptr, &srv))) {
				safeRelease(texture);
				safeRelease(srv);
				return false;
			}
			if (FAILED(device->CreateRenderTargetView(texture, nullptr, &rtv))) {
				safeRelease(texture);
				safeRelease(srv);
				safeRelease(rtv);
				return false;
			}
			return true;
		}

		void updateConstants(float directionX, float directionY) {
			BlurConstants constants = {};
			constants.texelSize[0] = targetWidth > 0 ? 1.f / static_cast<float>(targetWidth) : 0.f;
			constants.texelSize[1] = targetHeight > 0 ? 1.f / static_cast<float>(targetHeight) : 0.f;
			constants.direction[0] = directionX;
			constants.direction[1] = directionY;
			constants.radius = BlurRadius;

			context->UpdateSubresource(constantsBuffer, 0, nullptr, &constants, 0, 0);
			context->PSSetConstantBuffers(0, 1, &constantsBuffer);
		}

		void renderFullscreen(ID3D11ShaderResourceView* sourceSrv, ID3D11RenderTargetView* targetRtv, ID3D11PixelShader* pixelShader) {
			ID3D11ShaderResourceView* nullSrv = nullptr;
			ID3D11RenderTargetView* renderTarget = targetRtv;
			ID3D11Buffer* nullBuffer = nullptr;
			const UINT stride = 0;
			const UINT offset = 0;
			const float blendFactor[4] = {};

			context->PSSetShaderResources(0, 1, &nullSrv);
			context->OMSetRenderTargets(1, &renderTarget, nullptr);

			D3D11_VIEWPORT viewport = {};
			viewport.Width = static_cast<float>(targetWidth);
			viewport.Height = static_cast<float>(targetHeight);
			viewport.MinDepth = 0.f;
			viewport.MaxDepth = 1.f;
			context->RSSetViewports(1, &viewport);

			context->IASetInputLayout(nullptr);
			context->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
			context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->VSSetShader(vertexShader, nullptr, 0);
			context->PSSetShader(pixelShader, nullptr, 0);
			context->GSSetShader(nullptr, nullptr, 0);
			context->HSSetShader(nullptr, nullptr, 0);
			context->DSSetShader(nullptr, nullptr, 0);
			context->CSSetShader(nullptr, nullptr, 0);
			context->PSSetSamplers(0, 1, &samplerState);
			context->RSSetState(rasterizerState);
			context->OMSetBlendState(blendState, blendFactor, 0xffffffff);
			context->OMSetDepthStencilState(depthStencilState, 0);
			context->PSSetShaderResources(0, 1, &sourceSrv);
			context->Draw(3, 0);
			context->PSSetShaderResources(0, 1, &nullSrv);
		}
	};

	BlurPipeline blurPipeline;

	int normalizeAlpha(float alpha) {
		if (alpha <= 1.f)
			return std::clamp(static_cast<int>(alpha * 255.f), 0, 255);
		return std::clamp(static_cast<int>(alpha), 0, 255);
	}
}

void ImGuiRender::beginFrame(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer) {
	blurPipeline.beginFrame(device, context, backBuffer);
}

void ImGuiRender::endFrame() {
	blurPipeline.endFrame();
}

void ImGuiRender::releaseBlur() {
	blurPipeline.releaseAll();
}

namespace {
	ImTextureID externalBlurTexture = 0;
}

void ImGuiRender::setExternalBlurTexture(ImTextureID id) {
	externalBlurTexture = id;
}

void ImGuiRender::setDrawList(ImDrawList* d) {
	drawlist = d;
}

void ImGuiRender::drawText(Vec2<float> textPos, const std::string& textStr, UIColor color, float textSize, bool glow, float thick_ness) {
	drawlist->AddText(nullptr, fontSize * textSize, textPos.toImVec2(), color.toImColor(), textStr.c_str());
}

float ImGuiRender::getTextWidth(const std::string& textStr, float textSize) {
	return ImGui::GetFont()->CalcTextSizeA(fontSize * textSize, FLT_MAX, -1, textStr.c_str()).x;
}

float ImGuiRender::getTextHeight(float textSize) {
	return ImGui::GetFont()->CalcTextSizeA(fontSize * textSize, FLT_MAX, -1, "").y;
}

void ImGuiRender::drawLine(Vec2<float> start, Vec2<float> end, UIColor color, float width) {
	drawlist->AddLine(start.toImVec2(), end.toImVec2(), color.toImColor(), width);
}

void ImGuiRender::drawRectangle(const Vec4<float>& rectPos, UIColor color, float lineWidth, float rounding, ImDrawFlags flags) {
	drawlist->AddRect(ImVec2(rectPos.x, rectPos.y), ImVec2(rectPos.z, rectPos.w), color.toImColor(), rounding, flags, lineWidth);
}


void ImGuiRender::drawRectangle(Vec2<float> pMin, Vec2<float> pMax, UIColor color, float lineWidth, float rounding, ImDrawFlags flags) {
	drawlist->AddRect(pMin.toImVec2(), pMax.toImVec2(), color.toImColor(), rounding, flags, lineWidth);
}

void ImGuiRender::fillRectangle(Vec4<float> rectPos, UIColor color, float rounding, ImDrawFlags flags) {
	drawlist->AddRectFilled(ImVec2(rectPos.x, rectPos.y), ImVec2(rectPos.z, rectPos.w), color.toImColor(), rounding, flags);
}

void ImGuiRender::fillRectangle(Vec2<float> pMin, Vec2<float> pMax, UIColor color, float rounding, ImDrawFlags flags) {
	drawlist->AddRectFilled(pMin.toImVec2(), pMax.toImVec2(), color.toImColor(), rounding, flags);
}

void ImGuiRender::drawBlur(const Vec4<float>& rectPos, float rounding, float alpha, ImDrawFlags flags) {
	drawBlur(Vec2<float>(rectPos.x, rectPos.y), Vec2<float>(rectPos.z, rectPos.w), rounding, alpha, flags);
}

void ImGuiRender::drawBlur(Vec2<float> pMin, Vec2<float> pMax, float rounding, float alpha, ImDrawFlags flags) {
	if (!drawlist || pMax.x <= pMin.x || pMax.y <= pMin.y)
		return;

	ImTextureID blurTexture = externalBlurTexture;
	if (blurTexture == 0) {
		if (!blurPipeline.prepare())
			return;
		ID3D11ShaderResourceView* blurSrv = blurPipeline.getSrv();
		if (!blurSrv)
			return;
		blurTexture = (ImTextureID)(intptr_t)blurSrv;
	}

	const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	const float uvWidth = displaySize.x > 0.f ? displaySize.x : static_cast<float>(blurPipeline.sourceWidth);
	const float uvHeight = displaySize.y > 0.f ? displaySize.y : static_cast<float>(blurPipeline.sourceHeight);
	if (uvWidth <= 0.f || uvHeight <= 0.f)
		return;

	const ImVec2 uvMin(pMin.x / uvWidth, pMin.y / uvHeight);
	const ImVec2 uvMax(pMax.x / uvWidth, pMax.y / uvHeight);
	const ImTextureRef textureRef(blurTexture);
	const ImU32 tint = IM_COL32(255, 255, 255, normalizeAlpha(alpha));
	drawlist->AddImageRounded(textureRef, pMin.toImVec2(), pMax.toImVec2(), uvMin, uvMax, tint, rounding, flags);
}


bool ImGuiRender::worldToScreen(const Vec3<float>& pos, Vec2<float>& out) {
	ClientInstance* clientInstance = Game::clientInstance != nullptr
		? Game::clientInstance
		: ClientInstance::get();
	if (clientInstance == nullptr || clientInstance->levelRenderer == nullptr)
		return false;

	GuiData* guiData = clientInstance->getGuiData();
	if (guiData == nullptr)
		return false;

	LevelRendererPlayer* levelRendererPlayer = clientInstance->levelRenderer->getLevelRendererPlayer();
	if (levelRendererPlayer == nullptr)
		return false;

	GameRenderer* gameRenderer = clientInstance->minecraftGame != nullptr
		? clientInstance->minecraftGame->gameRenderer
		: nullptr;
	if (gameRenderer == nullptr)
		return false;

	const Vec2<float>& screenSize = guiData->screenSize;
	const Vec3<float> origin = levelRendererPlayer->getOrigin();

	const glm::vec4 relative(pos.x - origin.x, pos.y - origin.y, pos.z - origin.z, 1.0f);
	const glm::vec4 clipCoords = gameRenderer->lastProjectionMatrix._m * gameRenderer->lastViewMatrix._m * relative;
	if (clipCoords.w < 0.1f)
		return false;

	out.x = (clipCoords.x / clipCoords.w + 1.0f) * 0.5f * screenSize.x;
	out.y = (1.0f - clipCoords.y / clipCoords.w) * 0.5f * screenSize.y;
	return true;
}

bool ImGuiRender::drawBox3D(
	ImDrawList* drawList,
	const AABB& box,
	UIColor fillColor,
	UIColor lineColor,
	float lineWidth,
	float scale) {
	if (drawList == nullptr)
		return false;

	AABB renderBox = box;
	if (scale != 1.f) {
		const Vec3<float> center = box.getCenter();
		const Vec3<float> halfSize = box.upper.sub(box.lower).mul(
			Vec3<float>(scale * 0.5f, scale * 0.5f, scale * 0.5f));
		renderBox.lower = center.sub(halfSize);
		renderBox.upper = center.add(halfSize);
	}

	const std::array<Vec3<float>, 8> corners = {
		Vec3<float>(renderBox.lower.x, renderBox.lower.y, renderBox.lower.z),
		Vec3<float>(renderBox.upper.x, renderBox.lower.y, renderBox.lower.z),
		Vec3<float>(renderBox.lower.x, renderBox.lower.y, renderBox.upper.z),
		Vec3<float>(renderBox.upper.x, renderBox.lower.y, renderBox.upper.z),
		Vec3<float>(renderBox.lower.x, renderBox.upper.y, renderBox.lower.z),
		Vec3<float>(renderBox.upper.x, renderBox.upper.y, renderBox.lower.z),
		Vec3<float>(renderBox.lower.x, renderBox.upper.y, renderBox.upper.z),
		Vec3<float>(renderBox.upper.x, renderBox.upper.y, renderBox.upper.z)
	};

	std::array<ImVec2, 8> projected{};
	for (std::size_t index = 0; index < corners.size(); ++index) {
		Vec2<float> screen;
		if (!worldToScreen(corners[index], screen))
			return false;
		projected[index] = ImVec2(screen.x, screen.y);
	}

	if (fillColor.a > 0) {
		const std::vector<ImVec2> hull = buildConvexHull(projected);
		if (hull.size() >= 3)
			drawList->AddConvexPolyFilled(hull.data(), static_cast<int>(hull.size()), fillColor.toImColor());
	}

	if (lineColor.a > 0 && lineWidth > 0.f) {
		constexpr std::array<std::array<int, 2>, 12> edges = {{
			{{0, 1}}, {{1, 3}}, {{3, 2}}, {{2, 0}},
			{{4, 5}}, {{5, 7}}, {{7, 6}}, {{6, 4}},
			{{0, 4}}, {{1, 5}}, {{2, 6}}, {{3, 7}}
		}};
		for (const auto& edge : edges)
			drawList->AddLine(projected[edge[0]], projected[edge[1]], lineColor.toImColor(), lineWidth);
	}

	return true;
}

void ImGuiRender::renderCircle(Vec2<float> pos, ImColor color, float radius, float thickness, int num_seg) {
	drawlist->AddCircle(pos.toImVec2(), radius, color, num_seg, thickness);
}

void ImGuiRender::renderCircleFilled(Vec2<float> pos, ImColor color, float radius, int num_seg) {
	drawlist->AddCircleFilled(pos.toImVec2(), radius, color, num_seg);
}

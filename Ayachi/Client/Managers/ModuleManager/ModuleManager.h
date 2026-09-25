#pragma once

#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <string_view>

#include "Modules/ModuleBase/Module.h"

class ModuleManager {
public:
	static inline std::vector<Module*> moduleList;

	static void init();
	static void shutdown();
	static void updateEnabledCache();
	static Module* findModule(std::string_view name);

	template <typename TRet>
	static TRet* getModule() {
		static TRet* cached = []() -> TRet* {
			try {
				for (Module* mod : moduleList) {
					TRet* result = dynamic_cast<TRet*>(mod);
					if (result != nullptr)
						return result;
				}
			} catch (...) {}
			return nullptr;
		}();
		return cached;
	}

	static void onKeyUpdate(int key, bool isDown);
	static void onClientTick();
	static void onTick(Level* level);
	static void onSendPacket(Packet* packet, bool& cancel);
	static void onReceivePacket(Packet* packet, bool& cancel);
	static void onSendChatMessage(std::string& message, bool& cancel);
	static void onDisplayClientMessage(std::string& message, bool& cancel);
	static void onGetGamma(float& gamma);
	static void onGetFov(float& fov);
	static void onGetPerspective(int& perspective);
	static void onGetHideHand(bool& hideHand);
	static void onLevelRender(LevelRenderer* levelRenderer, ScreenContext* screenContext);
	static void onRenderOutlineSelection(
		const Block& block,
		const BlockSource& blockSource,
		const BlockPos& position,
		bool& cancel);
	static void onRenderHitSelect(const BlockPos& position, bool& cancel);
	static void onRenderItemInHand(
		ItemInHandRenderer* renderer,
		BaseActorRenderContext& renderContext,
		const Matrix& previousProjection,
		ItemContextFlags itemFlags,
		bool& cancel);
	static void onRenderItemInOffhand(
		ItemInHandRenderer* renderer,
		BaseActorRenderContext& renderContext,
		Player& player,
		ItemContextFlags itemFlags,
		bool& cancel);
	static void onGetTimeOfDay(float& time);
	static void onMCRender(MinecraftUIRenderContext* renderCtx);
	static void onDrawText(
		MinecraftUIRenderContext* renderCtx,
		Font& font,
		const RectangleArea& area,
		std::string& text,
		const mce::Color& color,
		float alpha,
		ui::TextAlignment alignment,
		const TextMeasureData& textData,
		const CaretMeasureData& caretData,
		bool& cancel);
	static void onDrawImage(
		MinecraftUIRenderContext* renderCtx,
		const mce::ClientTexture& texture,
		const glm::vec2& position,
		const glm::vec2& size,
		const glm::vec2& uv,
		const glm::vec2& uvSize,
		bool colorCorrected,
		bool& cancel);
	static void onDrawNineslice(
		MinecraftUIRenderContext* renderCtx,
		const mce::ClientTexture& texture,
		const NinesliceInfo& info,
		bool& cancel);
	static void onLoadConfig(void* conf);
	static void onSaveConfig(void* conf);
	static void onImGuiRender(ImDrawList* drawlist);

private:
	using ModuleList = std::vector<Module*>;

	static inline std::atomic<std::shared_ptr<const ModuleList>> enabledSnapshot{ std::make_shared<const ModuleList>() };
	static inline std::atomic<std::shared_ptr<const ModuleList>> backgroundSnapshot{ std::make_shared<const ModuleList>() };

	static inline std::atomic<std::shared_ptr<const ModuleList>> allModulesSnapshot{ std::make_shared<const ModuleList>() };

	static inline std::atomic<bool> batchUpdateMode{ false };
	static void dispatch(const std::function<void(Module*)>& callback);

	friend bool isInBatchUpdateMode();
};

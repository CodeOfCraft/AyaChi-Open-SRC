#pragma once

#include <cstddef>
#include <cstring>

#include "../Runtime/GameData.h"

class ActorRenderDispatcher;
class ClientInstance;
class ItemRenderer;
class MinecraftGame;
class ScreenContext;

class alignas(8) BaseActorRenderContext {
public:
	BaseActorRenderContext(
		ScreenContext* screenContext,
		ClientInstance* clientInstance,
		MinecraftGame* minecraftGame) {
		std::memset(storage, 0, sizeof(storage));
		if (GameData::Address::BaseActorRenderContext_Constructor == 0 ||
			screenContext == nullptr || clientInstance == nullptr || minecraftGame == nullptr)
			return;

		using Constructor = BaseActorRenderContext*(__fastcall*)(
			BaseActorRenderContext*, ScreenContext*, ClientInstance*, MinecraftGame*);
		reinterpret_cast<Constructor>(GameData::Address::BaseActorRenderContext_Constructor)(
			this, screenContext, clientInstance, minecraftGame);
	}

	~BaseActorRenderContext() {
		reset();
	}

	BaseActorRenderContext(const BaseActorRenderContext&) = delete;
	BaseActorRenderContext& operator=(const BaseActorRenderContext&) = delete;
	BaseActorRenderContext(BaseActorRenderContext&&) = delete;
	BaseActorRenderContext& operator=(BaseActorRenderContext&&) = delete;

	[[nodiscard]] bool isValid() const noexcept {
		return getVTable() != nullptr;
	}

	[[nodiscard]] ClientInstance* getClientInstance() const noexcept {
		return getPointer<ClientInstance>(0x18);
	}

	[[nodiscard]] MinecraftGame* getMinecraftGame() const noexcept {
		return getPointer<MinecraftGame>(0x20);
	}

	[[nodiscard]] ScreenContext* getScreenContext() const noexcept {
		return getPointer<ScreenContext>(0x28);
	}

	[[nodiscard]] ActorRenderDispatcher* getActorRenderDispatcher() const noexcept {
		return getPointer<ActorRenderDispatcher>(0x38);
	}

	[[nodiscard]] ItemRenderer* getItemRenderer() const noexcept {
		return getPointer<ItemRenderer>(0x58);
	}

private:
	[[nodiscard]] void** getVTable() const noexcept {
		return *reinterpret_cast<void***>(const_cast<BaseActorRenderContext*>(this));
	}

	void reset() noexcept {
		void** vtable = getVTable();
		if (vtable != nullptr && vtable[0] != nullptr) {
			using Destructor = void*(__fastcall*)(BaseActorRenderContext*, unsigned int);
			reinterpret_cast<Destructor>(vtable[0])(this, 0);
		}
		std::memset(storage, 0, sizeof(storage));
	}

	template <typename T>
	[[nodiscard]] T* getPointer(std::size_t offset) const noexcept {
		return *reinterpret_cast<T* const*>(
			reinterpret_cast<const std::byte*>(this) + offset);
	}

	std::byte storage[0xC8];
};

static_assert(sizeof(BaseActorRenderContext) == 0xC8);
static_assert(alignof(BaseActorRenderContext) == 8);

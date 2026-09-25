#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "Player.h"

class ClientInstance;
class ItemStack;
class Packet;

enum class PlayerRespawnState : uint8_t {
	SearchingForSpawn = 0,
	ReadyToSpawn = 1,
	ClientReadyToSpawn = 2,
};

class LocalPlayer : public Player {
public:
	static constexpr std::size_t NativeSize = 0x1158;

	LocalPlayer() = delete;

	float getPortalEffectTime() const noexcept { return field<float>(0xC70); }
	float getOldPortalEffectTime() const noexcept { return field<float>(0xC74); }
	ClientInstance* getClient() const noexcept { return field<ClientInstance*>(0xCB8); }
	int getCanCloseScreenOnHurtAfterTime() const noexcept { return field<int>(0xCC0); }
	Vec3<float>& getLastFrameDelta() noexcept { return field<Vec3<float>>(0xCC4); }
	const Vec3<float>& getLastFrameDelta() const noexcept { return field<Vec3<float>>(0xCC4); }
	bool getPortalEffectEnabled() const noexcept { return field<bool>(0xCD0); }

	ItemStack* getSentOffhandItem() noexcept { return &field<ItemStack>(0xCD8); }
	const ItemStack* getSentOffhandItem() const noexcept { return &field<ItemStack>(0xCD8); }
	ItemStack* getSentInventoryItem() noexcept { return &field<ItemStack>(0xD70); }
	const ItemStack* getSentInventoryItem() const noexcept { return &field<ItemStack>(0xD70); }
	int getSentSelectedSlot() const noexcept { return field<int>(0xE08); }
	uint64_t getSessionTickCount() const noexcept { return field<uint64_t>(0xE58); }

	bool wasDamagedByMobThisFrame() const noexcept { return field<bool>(0xE78); }
	bool getIsTeacherCached() const noexcept { return field<bool>(0xE79); }
	bool hasBeenInitialized() const noexcept { return field<bool>(0xE7A); }
	bool wasTransitionBlocking() const noexcept { return field<bool>(0xE7B); }

	std::string& getInventorySearchString() noexcept { return field<std::string>(0xE80); }
	const std::string& getInventorySearchString() const noexcept { return field<std::string>(0xE80); }
	std::string& getFurnaceSearchString() noexcept { return field<std::string>(0xEA0); }
	const std::string& getFurnaceSearchString() const noexcept { return field<std::string>(0xEA0); }
	std::string& getBlastFurnaceSearchString() noexcept { return field<std::string>(0xEC0); }
	const std::string& getBlastFurnaceSearchString() const noexcept { return field<std::string>(0xEC0); }
	std::string& getSmokerSearchString() noexcept { return field<std::string>(0xEE0); }
	const std::string& getSmokerSearchString() const noexcept { return field<std::string>(0xEE0); }

	ItemStack* getItemActivationItem() noexcept { return &field<ItemStack>(0xF70); }
	const ItemStack* getItemActivationItem() const noexcept { return &field<ItemStack>(0xF70); }
	int getItemActivationTicks() const noexcept { return field<int>(0x1008); }
	float getItemActivationOffsetX() const noexcept { return field<float>(0x100C); }
	float getItemActivationOffsetY() const noexcept { return field<float>(0x1010); }

	PlayerRespawnState getClientRespawnState() const noexcept {
		return field<PlayerRespawnState>(0x1050);
	}
	Vec3<float>& getClientRespawnPotentialPosition() noexcept { return field<Vec3<float>>(0x1054); }
	const Vec3<float>& getClientRespawnPotentialPosition() const noexcept {
		return field<Vec3<float>>(0x1054);
	}
	int getRenderChunkRadiusLowMemoryWatermark() const noexcept { return field<int>(0x1060); }
	std::string& getLastDeathInfo() noexcept { return field<std::string>(0x1068); }
	const std::string& getLastDeathInfo() const noexcept { return field<std::string>(0x1068); }
	float getCurrentExperienceCache() const noexcept { return field<float>(0x1098); }
	int getCurrentLevelCache() const noexcept { return field<int>(0x109C); }
	bool hasPreparedMainChunkSource() const noexcept { return field<bool>(0x10B0); }

	void openInventory();
	bool isLoading() const;
	void stopLoading();

	void respawn();
	void resetRot();
	bool isAutoJumpEnabled() const;
	uint32_t getUserId() const;
	bool isInTrialMode();
	void addExperience(int experience);
	void addLevels(int levels);
	bool isTeacher() const;

	void sendNetworkPacket(Packet& packet) const;
	uint8_t getMaxChunkBuildRadius() const;

	void _applyTurnDelta(const Vec2<float>& delta);
	void applyTurnDelta(const Vec2<float>& delta);

private:
	template <typename T>
	T& field(std::size_t offset) noexcept {
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	template <typename T>
	const T& field(std::size_t offset) const noexcept {
		return *reinterpret_cast<const T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}
};

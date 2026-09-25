#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

#include "../../../Utils/Maths.h"
#include "../../../Utils/MemoryUtil.h"
#include "ActorId.h"
#include "ActorSwingSource.h"
#include "ActorCategory.h"
#include "ActorDimensionRef.h"
#include "../Entity/EntityContext.h"
#include "../Entity/Components/AABBShapeComponent.h"
#include "../Entity/Components/ActorDiedComponent.h"
#include "../Entity/Components/ActorGameTypeComponent.h"
#include "../Entity/Components/ActorHeadRotationComponent.h"
#include "../Entity/Components/ActorRotationComponent.h"
#include "../Entity/Components/ActorTypeComponent.h"
#include "../Entity/Components/ActorUniqueIDComponent.h"
#include "../Entity/Components/ActorWalkAnimationComponent.h"
#include "../Entity/Components/AgeableComponent.h"
#include "../Entity/Components/ApplyGravityComponent.h"
#include "../Entity/Components/AttackAnimationComponent.h"
#include "../Entity/Components/FallDistanceComponent.h"
#include "../Entity/Components/FreezeImmuneFlagComponent.h"
#include "../Entity/Components/HorizontalCollisionFlagComponent.h"
#include "../Entity/Components/IsDeadFlagComponent.h"
#include "../Entity/Components/JumpTicksComponent.h"
#include "../Entity/Components/MobHurtTimeComponent.h"
#include "../Entity/Components/MobBodyRotationComponent.h"
#include "../Entity/Components/MovementSpeedComponent.h"
#include "../Entity/Components/OnFireComponent.h"
#include "../Entity/Components/OnGroundFlagComponent.h"
#include "../Entity/Components/RenderRotationComponent.h"
#include "../Entity/Components/RuntimeIDComponent.h"
#include "../Entity/Components/StateVectorComponent.h"
#include "../Entity/Components/SneakingComponent.h"
#include "../Entity/Components/VerticalCollisionFlagComponent.h"
#include "../Entity/Components/WasOnGroundFlagComponent.h"

class Dimension;
class ItemStack;
class Level;

class Actor {
public:
	static constexpr std::size_t NativeSize = 0x3B0;

	Actor() = delete;

	EntityContext& entityContext() {
		return field<EntityContext>(0x8);
	}
	const EntityContext& entityContext() const {
		return field<EntityContext>(0x8);
	}
	EntityContext& getEntityContext() { return entityContext(); }
	const EntityContext& getEntityContext() const { return entityContext(); }
	EntityId getEntityId() const noexcept { return entityContext().getEntityId(); }

	CLASS_MEMBER(std::string, alias, 0xB0);
	CLASS_MEMBER(Vec3<float>, sentDelta, 0x160);
	CLASS_MEMBER(float, scale, 0x16C);
	CLASS_MEMBER(float, previousScale, 0x170);
	CLASS_MEMBER(bool, ignoreLighting, 0x174);
	CLASS_MEMBER(bool, filterLighting, 0x175);
	CLASS_MEMBER(float, stepSoundVolume, 0x178);
	CLASS_MEMBER(float, stepSoundPitch, 0x17C);
	CLASS_MEMBER(AABB*, lastHitBoundingBox, 0x180);
	CLASS_MEMBER(uint64_t, nameTagHash, 0x188);
	CLASS_MEMBER(float, shadowOffset, 0x190);
	CLASS_MEMBER(float, pushThrough, 0x194);
	CLASS_MEMBER(int32_t, ticksExisted, 0x198);
	CLASS_MEMBER(int32_t, invulnerableTime, 0x19C);
	CLASS_MEMBER(int32_t, lastHealth, 0x1A0);
	CLASS_MEMBER(bool, invulnerable, 0x1A4);
	CLASS_MEMBER(int32_t, flameTextureFrameIndex, 0x1A8);
	CLASS_MEMBER(float, flameFrameIncrementTime, 0x1AC);
	CLASS_MEMBER(bool, alwaysFireImmune, 0x1B0);
	CLASS_MEMBER(bool, inheritRotationWhenRiding, 0x1B1);
	CLASS_MEMBER(bool, forcedLoading, 0x1B2);
	CLASS_MEMBER(bool, highlightedThisFrame, 0x1B3);
	CLASS_MEMBER(bool, initialized, 0x1B4);
	CLASS_MEMBER(bool, processedOnChunkDiscard, 0x1B5);
	CLASS_MEMBER(float, soundVolume, 0x1B8);
	CLASS_MEMBER(int32_t, shakeTime, 0x1BC);

	CLASS_MEMBER(ActorUniqueID, legacyUniqueID, 0x1C0);
	CLASS_MEMBER(Level*, level, 0x1D8);
	CLASS_MEMBER(ActorCategory, categories, 0x210);
	CLASS_MEMBER(uint32_t, actorVersion, 0x214);
	CLASS_MEMBER(StateVectorComponent*, stateVector, 0x218);
	CLASS_MEMBER(AABBShapeComponent*, aabbShape, 0x220);
	CLASS_MEMBER(ActorRotationComponent*, actorRotation, 0x228);

	CLASS_MEMBER(bool, changed, 0x268);
	CLASS_MEMBER(bool, removed, 0x269);
	CLASS_MEMBER(bool, movedToLimbo, 0x26A);
	CLASS_MEMBER(bool, movedToUnloadedChunk, 0x26B);
	CLASS_MEMBER(bool, blocksBuilding, 0x26C);
	CLASS_MEMBER(ActorUniqueID, targetID, 0x290);
	CLASS_MEMBER(ActorUniqueID, inLovePartnerID, 0x298);
	CLASS_MEMBER(bool, persistingTrade, 0x2AC);
	CLASS_MEMBER(bool, effectsDirty, 0x2AD);
	CLASS_MEMBER(bool, lootDropped, 0x2AE);
	CLASS_MEMBER(bool, loadedFromNBTThisFrame, 0x2AF);

	CLASS_MEMBER(std::string, filteredNameTag, 0x2E0);
	CLASS_MEMBER(bool, wasHurtThisTick, 0x318);
	CLASS_MEMBER(bool, wasHurtLastTick, 0x319);
	CLASS_MEMBER(ActorUniqueID, lastHurtMobID, 0x320);
	CLASS_MEMBER(ActorUniqueID, lastHurtByMobID, 0x328);
	CLASS_MEMBER(ActorUniqueID, lastHurtByPlayerID, 0x330);
	CLASS_MEMBER(uint64_t, lastHurtTimestamp, 0x338);
	CLASS_MEMBER(int32_t, lastHurtCause, 0x340);
	CLASS_MEMBER(float, lastHurtAmount, 0x344);
	CLASS_MEMBER(bool, predictableProjectile, 0x354);
	CLASS_MEMBER(bool, renderingInUI, 0x355);
	CLASS_MEMBER(bool, onScreen, 0x356);
	CLASS_MEMBER(bool, updateBonesAndEffects, 0x357);
	CLASS_MEMBER(bool, updateEffects, 0x358);
	CLASS_MEMBER(bool, canPickupItems, 0x359);
	CLASS_MEMBER(bool, hasSetCanPickupItems, 0x35A);
	CLASS_MEMBER(bool, chainedDamageEffects, 0x35B);
	CLASS_MEMBER(ActorUniqueID, fishingHookID, 0x3A8);

	ActorDimensionRef& getDimensionRef() noexcept { return field<ActorDimensionRef>(0x1C8); }
	const ActorDimensionRef& getDimensionRef() const noexcept { return field<ActorDimensionRef>(0x1C8); }
	Level& getLevel() const;
	bool hasCategory(ActorCategory category) const noexcept;
	uint64_t getRuntimeID() const;
	uint32_t getEntityTypeID() const;
	bool isPlayer() const;
	bool isItem() const;
	bool isOnGround() const;

	AABB& getBoundingBox();
	const AABB& getBoundingBox() const;
	Vec2<float>& getRot();
	const Vec2<float>& getRot() const;
	Vec3<float>& getVelocity();
	const Vec3<float>& getVelocity() const;
	Vec3<float>& getPos();
	const Vec3<float>& getPos() const;
	Vec3<float>& getPosOld();
	const Vec3<float>& getPosOld() const;
	Vec3<float> getFiringPos() const;
	Vec3<float> getHeadLookVector(float alpha = 0.0f) const;
	float getInterpolatedBodyRot(float alpha = 0.0f) const;
	float getInterpolatedHeadRot(float alpha = 0.0f) const;
	float getInterpolatedBodyYaw(float alpha = 0.0f) const;
	float getYawSpeedInDegreesPerSecond() const;
	void teleportTo(const Vec3<float>& pos, bool stopRiding, int cause, int sourceEntityType, bool keepVelocity);
	void lerpMotion(const Vec3<float>& delta);

	std::string getNameTag() const;
	void setNameTag(const std::string& nametag);
	void setNameTag(std::string* nametag);
	std::string getEntityLocNameString() const;
	bool canShowNameTag() const;
	float getShadowRadius() const;
	float getMapDecorationRotation() const;

	void remove();
	void normalTick();
	void baseTick();
	void passengerTick();
	void despawn();
	void kill();
	bool shouldDropDeathLoot() const;
	bool hasDiedBefore() const;

	bool startRiding(Actor& vehicle, bool forceRiding);
	void addPassenger(Actor& passenger);
	bool canAddPassenger(Actor& passenger) const;
	float getPassengerYRotation(const Actor& passenger) const;

	bool isFireImmune() const;
	bool isInvisible() const;
	bool canInteractWithOtherEntitiesInGame() const;
	bool isImmobile() const;
	bool isSilentObserver() const;
	bool isSleeping() const;
	void setSleeping(bool sleeping);
	void setSneaking(bool sneaking);
	bool isBlocking() const;
	bool isAlive() const;
	bool isOnFire() const;
	bool isSurfaceMob() const;
	void setSitting(bool sitting);
	void setStanding(bool standing);
	bool canPowerJump() const;
	bool isEnchanted() const;
	bool canFreeze() const;
	bool canChangeDimensionsUsingPortal() const;
	void startSwimming();
	void stopSwimming();

	bool isTargetable() const;
	void setTarget(Actor* target);
	bool isValidTarget(Actor* attacker) const;
	bool swing(ActorSwingSource source);
	void swing();
	void playAmbientSound();
	void onLightningHit();
	void feed(int itemId);
	void handleEntityEvent(uint8_t eventId, int data);

	void setCarriedItem(const ItemStack& item);
	const ItemStack& getCarriedItem() const;
	void setOffhandSlot(const ItemStack& item);
	bool canPickupItem(const ItemStack& item) const;
	bool add(ItemStack& item);
	bool drop(const ItemStack& item, bool randomly);

	void buildDebugInfo(std::string& out) const;
	bool canObstructSpawningAndBlockPlacement() const;

	template<typename T>
	T* tryGetComponent() {
		return entityContext().tryGetComponent<T>();
	}

	template<typename T>
	const T* tryGetComponent() const {
		return entityContext().tryGetComponent<T>();
	}

	template<typename T>
	bool hasComponent() const {
		return entityContext().hasComponent<T>();
	}

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

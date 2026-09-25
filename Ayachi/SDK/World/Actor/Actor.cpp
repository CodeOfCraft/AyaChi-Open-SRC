#include "Actor.h"
#include "Player/Player.h"

AABB& Actor::getBoundingBox() {
	return aabbShape->boundingBox;
}

const AABB& Actor::getBoundingBox() const {
	return field<AABBShapeComponent*>(0x220)->boundingBox;
}

Vec2<float>& Actor::getRot() {
	return actorRotation->rotation;
}

const Vec2<float>& Actor::getRot() const {
	return field<ActorRotationComponent*>(0x228)->rotation;
}

Vec3<float>& Actor::getVelocity() {
	return stateVector->velocity;
}

const Vec3<float>& Actor::getVelocity() const {
	return field<StateVectorComponent*>(0x218)->velocity;
}

Vec3<float>& Actor::getPos() {
	return stateVector->pos;
}

const Vec3<float>& Actor::getPos() const {
	return field<StateVectorComponent*>(0x218)->pos;
}

Vec3<float>& Actor::getPosOld() {
	return stateVector->posOld;
}

const Vec3<float>& Actor::getPosOld() const {
	return field<StateVectorComponent*>(0x218)->posOld;
}

Level& Actor::getLevel() const {
	return *field<Level*>(0x1D8);
}

bool Actor::hasCategory(ActorCategory category) const noexcept {
	return (field<ActorCategory>(0x210) & category) == category;
}

std::string Actor::getNameTag() const {
	if (Addresses::Actor_getNameTag != 0) {
		using func_t = std::string const& (__fastcall*)(Actor const*);
		const std::string& nameTag = reinterpret_cast<func_t>(Addresses::Actor_getNameTag)(this);
		return nameTag;
	}

	if (isPlayer())
		return static_cast<Player*>(const_cast<Actor*>(this))->playerName;

	return {};
}

void Actor::setNameTag(const std::string& nametag) {
	if (Addresses::Actor_setNameTag == 0)
		return;

	using Function = void(__fastcall*)(Actor*, const std::string&);
	reinterpret_cast<Function>(Addresses::Actor_setNameTag)(this, nametag);
}

void Actor::setNameTag(std::string* nametag) {
	if (nametag != nullptr)
		setNameTag(*nametag);
}

uint64_t Actor::getRuntimeID() const {
	const RuntimeIDComponent* comp = tryGetComponent<RuntimeIDComponent>();
	return comp ? comp->runtimeID : 0;
}

uint32_t Actor::getEntityTypeID() const {
	const ActorTypeComponent* comp = tryGetComponent<ActorTypeComponent>();
	return comp ? comp->type : 0;
}

bool Actor::isPlayer() const {
	return getEntityTypeID() == 319;
}

bool Actor::isItem() const {
	return getEntityTypeID() == 64;
}

bool Actor::isOnGround() const {
	return entityContext().enttRegistry.all_of<OnGroundFlagComponent>(entityContext().entity);
}

void Actor::remove() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::remove, void>(this);
}

Vec3<float> Actor::getFiringPos() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getFiringPos, Vec3<float>>(
		const_cast<Actor*>(this));
}

float Actor::getInterpolatedBodyRot(float alpha) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getInterpolatedBodyRot, float, float>(
		const_cast<Actor*>(this), alpha);
}

float Actor::getInterpolatedHeadRot(float alpha) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getInterpolatedHeadRot, float, float>(
		const_cast<Actor*>(this), alpha);
}

float Actor::getInterpolatedBodyYaw(float alpha) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getInterpolatedBodyYaw, float, float>(
		const_cast<Actor*>(this), alpha);
}

float Actor::getYawSpeedInDegreesPerSecond() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getYawSpeedInDegreesPerSecond, float>(
		const_cast<Actor*>(this));
}

bool Actor::isFireImmune() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isFireImmune, bool>(
		const_cast<Actor*>(this));
}

void Actor::teleportTo(
	const Vec3<float>& pos,
	bool stopRiding,
	int cause,
	int sourceEntityType,
	bool keepVelocity) {
	MemoryUtil::CallVFunc<
		GameData::VTableIndex::Actor::teleportTo,
		void,
		const Vec3<float>&,
		bool,
		int,
		int,
		bool>(this, pos, stopRiding, cause, sourceEntityType, keepVelocity);
}

void Actor::lerpMotion(const Vec3<float>& delta) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::lerpMotion, void, const Vec3<float>&>(this, delta);
}

void Actor::normalTick() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::normalTick, void>(this);
}

void Actor::baseTick() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::baseTick, void>(this);
}

void Actor::passengerTick() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::passengerTick, void>(this);
}

bool Actor::startRiding(Actor& vehicle, bool forceRiding) {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::startRiding, bool, Actor&, bool>(
		this, vehicle, forceRiding);
}

void Actor::addPassenger(Actor& passenger) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::addPassenger, void, Actor&>(this, passenger);
}

std::string Actor::getEntityLocNameString() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getEntityLocNameString, std::string>(
		const_cast<Actor*>(this));
}

bool Actor::isInvisible() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isInvisible, bool>(
		const_cast<Actor*>(this));
}

bool Actor::canShowNameTag() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::canShowNameTag, bool>(
		const_cast<Actor*>(this));
}

float Actor::getShadowRadius() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getShadowRadius, float>(
		const_cast<Actor*>(this));
}

Vec3<float> Actor::getHeadLookVector(float alpha) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getHeadLookVector, Vec3<float>, float>(
		const_cast<Actor*>(this), alpha);
}

bool Actor::canInteractWithOtherEntitiesInGame() const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Actor::canInteractWithOtherEntitiesInGame,
		bool>(const_cast<Actor*>(this));
}

bool Actor::isImmobile() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isImmobile, bool>(
		const_cast<Actor*>(this));
}

bool Actor::isSilentObserver() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isSilentObserver, bool>(
		const_cast<Actor*>(this));
}

bool Actor::isSleeping() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isSleeping, bool>(
		const_cast<Actor*>(this));
}

void Actor::setSleeping(bool sleeping) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::setSleeping, void, bool>(this, sleeping);
}

void Actor::setSneaking(bool sneaking) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::setSneaking, void, bool>(this, sneaking);
}

bool Actor::isBlocking() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isBlocking, bool>(
		const_cast<Actor*>(this));
}

bool Actor::isAlive() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isAlive, bool>(
		const_cast<Actor*>(this));
}

bool Actor::isOnFire() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isOnFire, bool>(
		const_cast<Actor*>(this));
}

bool Actor::isSurfaceMob() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isSurfaceMob, bool>(
		const_cast<Actor*>(this));
}

bool Actor::isTargetable() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isTargetable, bool>(
		const_cast<Actor*>(this));
}

void Actor::setTarget(Actor* target) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::setTarget, void, Actor*>(this, target);
}

bool Actor::isValidTarget(Actor* attacker) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isValidTarget, bool, Actor*>(
		const_cast<Actor*>(this), attacker);
}

void Actor::setSitting(bool sitting) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::setSitting, void, bool>(this, sitting);
}

void Actor::setStanding(bool standing) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::setStanding, void, bool>(this, standing);
}

bool Actor::canPowerJump() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::canPowerJump, bool>(
		const_cast<Actor*>(this));
}

bool Actor::isEnchanted() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::isEnchanted, bool>(
		const_cast<Actor*>(this));
}

void Actor::playAmbientSound() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::playAmbientSound, void>(this);
}

void Actor::onLightningHit() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::onLightningHit, void>(this);
}

void Actor::feed(int itemId) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::feed, void, int>(this, itemId);
}

void Actor::handleEntityEvent(uint8_t eventId, int data) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::handleEntityEvent, void, uint8_t, int>(
		this, eventId, data);
}

void Actor::despawn() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::despawn, void>(this);
}

void Actor::setCarriedItem(const ItemStack& item) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::setCarriedItem, void, const ItemStack&>(
		this, item);
}

const ItemStack& Actor::getCarriedItem() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getCarriedItem, const ItemStack&>(
		const_cast<Actor*>(this));
}

void Actor::setOffhandSlot(const ItemStack& item) {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::setOffhandSlot, void, const ItemStack&>(
		this, item);
}

bool Actor::canFreeze() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::canFreeze, bool>(
		const_cast<Actor*>(this));
}

bool Actor::canChangeDimensionsUsingPortal() const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Actor::canChangeDimensionsUsingPortal,
		bool>(const_cast<Actor*>(this));
}

bool Actor::canAddPassenger(Actor& passenger) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::canAddPassenger, bool, Actor&>(
		const_cast<Actor*>(this), passenger);
}

bool Actor::canPickupItem(const ItemStack& item) const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::canPickupItem, bool, const ItemStack&>(
		const_cast<Actor*>(this), item);
}

void Actor::startSwimming() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::startSwimming, void>(this);
}

void Actor::stopSwimming() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::stopSwimming, void>(this);
}

void Actor::buildDebugInfo(std::string& out) const {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::buildDebugInfo, void, std::string&>(
		const_cast<Actor*>(this), out);
}

bool Actor::canObstructSpawningAndBlockPlacement() const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Actor::canObstructSpawningAndBlockPlacement,
		bool>(const_cast<Actor*>(this));
}

bool Actor::swing(ActorSwingSource source) {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::swing, bool, ActorSwingSource>(
		this, source);
}

void Actor::swing() {
	swing(ActorSwingSource::None);
}

float Actor::getMapDecorationRotation() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::getMapDecorationRotation, float>(
		const_cast<Actor*>(this));
}

float Actor::getPassengerYRotation(const Actor& passenger) const {
	return MemoryUtil::CallVFunc<
		GameData::VTableIndex::Actor::getPassengerYRotation,
		float,
		const Actor&>(const_cast<Actor*>(this), passenger);
}

bool Actor::add(ItemStack& item) {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::add, bool, ItemStack&>(this, item);
}

bool Actor::drop(const ItemStack& item, bool randomly) {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::drop, bool, const ItemStack&, bool>(
		this, item, randomly);
}

void Actor::kill() {
	MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::kill, void>(this);
}

bool Actor::shouldDropDeathLoot() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::shouldDropDeathLoot, bool>(
		const_cast<Actor*>(this));
}

bool Actor::hasDiedBefore() const {
	return MemoryUtil::CallVFunc<GameData::VTableIndex::Actor::hasDiedBefore, bool>(
		const_cast<Actor*>(this));
}

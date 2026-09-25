#include "ItemRendererRenderHook.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <unordered_map>

#include "../../../../Client.h"
#include "../../../ModuleManager/ModuleManager.h"
#include "../../../ModuleManager/Modules/Category/Render/ItemPhysics.h"
#include "../../../../../SDK/Render/ActorRenderData.h"
#include "../../../../../SDK/Render/BaseActorRenderContext.h"
#include "../../../../../SDK/Render/ItemRenderer.h"
#include "../../../../../SDK/Render/ScreenContext.h"
#include "../../../../../SDK/World/Actor/Actor.h"
#include "../../../../../Utils/HookGuard.h"

namespace {
constexpr float cleanupInterval = 2.f;
constexpr float stateLifetime = 6.f;
constexpr std::size_t isInItemFrameOffset = 0x460;

class RenderStateGuard {
public:
	RenderStateGuard(ActorRenderData* data, MatrixStack* matrixStack, bool* itemFrameState)
		: data(data), matrixStack(matrixStack), itemFrameState(itemFrameState),
		originalPosition(data->position), originalItemFrameState(*itemFrameState),
		originalDirtyState(matrixStack->isDirty) {
		matrixStack->stack.push_back(matrixStack->stack.back());
		matrixStack->isDirty = true;
	}

	~RenderStateGuard() {
		data->position = originalPosition;
		*itemFrameState = originalItemFrameState;
		matrixStack->stack.pop_back();
		matrixStack->isDirty = originalDirtyState;
	}

	[[nodiscard]] const Vec3<float>& getOriginalPosition() const noexcept {
		return originalPosition;
	}

private:
	ActorRenderData* data;
	MatrixStack* matrixStack;
	bool* itemFrameState;
	Vec3<float> originalPosition;
	bool originalItemFrameState;
	bool originalDirtyState;
};

struct ItemAnimationState {
	float spinX = 0.f;
	float spinY = 0.f;
	float spinZ = 0.f;
	float groundBlend = 0.f;
	float bobPhase = 0.f;
	float landingWobble = 0.f;
	float fallSpeed = 0.f;
	float slideSpin = 0.f;
	bool wasOnGround = false;
	std::chrono::steady_clock::time_point lastSeen;
};

static float clamp01(float value) {
	return std::clamp(value, 0.f, 1.f);
}

static float wrapDegrees(float degrees) {
	degrees = std::fmod(degrees, 360.f);
	return degrees < 0.f ? degrees + 360.f : degrees;
}

static float lerp(float start, float end, float amount) {
	return start + (end - start) * amount;
}

static float lerpDegrees(float start, float end, float amount) {
	const float delta = std::fmod(end - start + 540.f, 360.f) - 180.f;
	return wrapDegrees(start + delta * amount);
}

static float smoothstep(float value) {
	value = clamp01(value);
	return value * value * (3.f - 2.f * value);
}

static float approach(float current, float target, float speed, float deltaTime) {
	return lerp(current, target, 1.f - std::exp(-speed * deltaTime));
}

static float getGroundRotation(std::uintptr_t actorAddress) {
	std::uint64_t hash = static_cast<std::uint64_t>(actorAddress >> 4);
	hash ^= hash >> 33;
	hash *= 0xff51afd7ed558ccdULL;
	hash ^= hash >> 33;
	return static_cast<float>(hash % 360ULL);
}

static float getHashedAngle(std::uintptr_t actorAddress, std::uint64_t salt) {
	std::uint64_t hash = static_cast<std::uint64_t>(actorAddress >> 4) ^ salt;
	hash ^= hash >> 33;
	hash *= 0xff51afd7ed558ccdULL;
	hash ^= hash >> 33;
	hash *= 0xc4ceb9fe1a85ec53ULL;
	hash ^= hash >> 33;
	return static_cast<float>(hash % 360ULL);
}

static ItemAnimationState& getAnimationState(
	std::uintptr_t actorAddress,
	bool onGround,
	const std::chrono::steady_clock::time_point& now) {
	static thread_local std::unordered_map<std::uintptr_t, ItemAnimationState> states;
	static thread_local auto lastCleanup = std::chrono::steady_clock::now();

	if (std::chrono::duration<float>(now - lastCleanup).count() > cleanupInterval) {
		for (auto it = states.begin(); it != states.end();) {
			if (std::chrono::duration<float>(now - it->second.lastSeen).count() > stateLifetime)
				it = states.erase(it);
			else
				++it;
		}
		lastCleanup = now;
	}

	auto [it, inserted] = states.try_emplace(actorAddress);
	ItemAnimationState& state = it->second;
	if (inserted) {
		state.spinX = getHashedAngle(actorAddress, 0x45d9f3bULL);
		state.spinY = getHashedAngle(actorAddress, 0x9e3779b97f4a7c15ULL);
		state.spinZ = getHashedAngle(actorAddress, 0xbf58476d1ce4e5b9ULL);
		state.groundBlend = onGround ? 1.f : 0.f;
		state.bobPhase = glm::radians(getHashedAngle(actorAddress, 0x94d049bb133111ebULL));
		state.wasOnGround = onGround;
	}
	return state;
}
}

ItemRendererRenderHook::func_t ItemRendererRenderHook::oFunc = nullptr;

void ItemRendererRenderHook::renderCallback(
	ItemRenderer* renderer,
	BaseActorRenderContext& renderContext,
	ActorRenderData& renderData) {
	HookGuard::Scope guard;
	if (!oFunc)
		return;

	if (!guard || !Client::isInitialized() || renderData.mActor == nullptr || renderData.mIsInUI) {
		oFunc(renderer, renderContext, renderData);
		return;
	}

	static ItemPhysics* itemPhysics = nullptr;
	if (itemPhysics == nullptr)
		itemPhysics = ModuleManager::getModule<ItemPhysics>();
	if (itemPhysics == nullptr || !itemPhysics->isEnabled()) {
		oFunc(renderer, renderContext, renderData);
		return;
	}
	if (!renderData.mActor->isItem()) {
		oFunc(renderer, renderContext, renderData);
		return;
	}

	ScreenContext* screenContext = renderContext.getScreenContext();
	mce::Camera* camera = screenContext != nullptr ? screenContext->getCamera() : nullptr;
	MatrixStack* matrixStack = camera != nullptr ? camera->getWorldMatrixStack() : nullptr;
	if (matrixStack == nullptr || matrixStack->stack.empty()) {
		oFunc(renderer, renderContext, renderData);
		return;
	}
	Actor* actor = renderData.mActor;
	const std::uintptr_t actorAddress = reinterpret_cast<std::uintptr_t>(actor);
	const bool onGround = actor->isOnGround();
	const auto now = std::chrono::steady_clock::now();
	ItemAnimationState& animation = getAnimationState(actorAddress, onGround, now);

	float deltaTime = 0.f;
	if (animation.lastSeen.time_since_epoch().count() != 0)
		deltaTime = std::chrono::duration<float>(now - animation.lastSeen).count();
	animation.lastSeen = now;
	deltaTime = std::clamp(deltaTime, 0.f, 0.05f);

	Vec3<float> velocity;
	if (actor->stateVector != nullptr)
		velocity = actor->stateVector->velocity;

	const float horizontalSpeed = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	const float motionSpeed = std::sqrt(
		velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
	if (!onGround)
		animation.fallSpeed = std::max(animation.fallSpeed, std::max(0.f, -velocity.y));
	if (onGround && !animation.wasOnGround) {
		animation.landingWobble = std::clamp(animation.fallSpeed * 18.f, 3.f, 18.f);
		animation.fallSpeed = 0.f;
	}
	animation.wasOnGround = onGround;

	animation.groundBlend = approach(
		animation.groundBlend, onGround ? 1.f : 0.f, onGround ? 9.f : 14.f, deltaTime);
	const float airAmount = 1.f - smoothstep(animation.groundBlend);
	const float spinRate = (85.f + motionSpeed * 260.f) * itemPhysics->spinSpeed;
	animation.spinX = wrapDegrees(animation.spinX + deltaTime * spinRate * (0.75f + airAmount * 0.45f));
	animation.spinY = wrapDegrees(animation.spinY + deltaTime * spinRate * (0.35f + airAmount * 0.25f));
	animation.spinZ = wrapDegrees(animation.spinZ + deltaTime * spinRate * (0.55f + airAmount * 0.35f));
	animation.slideSpin = wrapDegrees(
		animation.slideSpin + deltaTime * horizontalSpeed * 150.f * itemPhysics->spinSpeed);
	animation.bobPhase += deltaTime * (3.5f + motionSpeed * 3.f);
	animation.landingWobble = approach(animation.landingWobble, 0.f, 5.5f, deltaTime);

	bool* isInItemFrame = reinterpret_cast<bool*>(actorAddress + isInItemFrameOffset);
	RenderStateGuard stateGuard(&renderData, matrixStack, isInItemFrame);
	glm::mat4& matrix = matrixStack->stack.back()._m;
	matrix = glm::translate(matrix, stateGuard.getOriginalPosition().toGlm());

	const float groundedAmount = smoothstep(animation.groundBlend);
	const float bob = std::sin(animation.bobPhase) * 0.025f * airAmount;
	const float settle = std::sin(animation.bobPhase * 4.2f) * animation.landingWobble * 0.0015f;
	const float height = lerp(
		itemPhysics->airOffset + bob, itemPhysics->groundOffset + settle, groundedAmount);
	const float wobble = std::sin(animation.bobPhase * 4.2f) * animation.landingWobble;
	const float pitch = lerpDegrees(animation.spinX, 90.f + wobble, groundedAmount);
	const float yaw = lerpDegrees(animation.spinY, wobble * 0.15f, groundedAmount);
	const float roll = lerpDegrees(
		animation.spinZ,
		getGroundRotation(actorAddress) + animation.slideSpin + wobble * 0.25f,
		groundedAmount);

	matrix = glm::translate(matrix, glm::vec3(0.f, height, 0.f));
	matrix = glm::rotate(matrix, glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
	matrix = glm::rotate(matrix, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
	matrix = glm::rotate(matrix, glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));

	renderData.position = Vec3<float>();
	*isInItemFrame = true;
	oFunc(renderer, renderContext, renderData);
}

ItemRendererRenderHook::ItemRendererRenderHook() {
	OriginFunc = reinterpret_cast<void*>(&oFunc);
	func = reinterpret_cast<void*>(&renderCallback);
}

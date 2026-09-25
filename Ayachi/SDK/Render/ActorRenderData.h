#pragma once

#include <cstddef>
#include <cstdint>

#include "../../Utils/Maths.h"

class Actor;
class ItemStack;

struct ActorRenderData {
	Actor* mActor;
	const ItemStack* mItemStack;
	Vec3<float> position;
	Vec2<float> rotation;
	Vec2<float> mHeadRot;
	bool glint;
	bool mIgnoreLighting;
	bool mIsInUI;
	float mDeltaTime;
	void* mAnimationComponent;
	void* mVariables;
	std::uint64_t mCSSMetadata;
};

static_assert(offsetof(ActorRenderData, mActor) == 0x0);
static_assert(offsetof(ActorRenderData, mItemStack) == 0x8);
static_assert(offsetof(ActorRenderData, position) == 0x10);
static_assert(offsetof(ActorRenderData, rotation) == 0x1C);
static_assert(offsetof(ActorRenderData, mHeadRot) == 0x24);
static_assert(offsetof(ActorRenderData, mIsInUI) == 0x2E);
static_assert(offsetof(ActorRenderData, mDeltaTime) == 0x30);
static_assert(offsetof(ActorRenderData, mAnimationComponent) == 0x38);
static_assert(sizeof(ActorRenderData) == 0x50);

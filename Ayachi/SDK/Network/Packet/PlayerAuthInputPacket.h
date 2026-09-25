#pragma once

#include <array>
#include <bitset>

#include "Packet.h"
#include "PlayerInputTypes.h"
#include "../../World/Actor/ActorId.h"
#include "../../../Utils/Maths.h"

enum class InputData : int {
	Ascend = 0,
	Descend = 1,
	NorthJumpDeprecated = 2,
	JumpDown = 3,
	SprintDown = 4,
	ChangeHeight = 5,
	Jumping = 6,
	AutoJumpingInWater = 7,
	Sneaking = 8,
	SneakDown = 9,
	Up = 10,
	Down = 11,
	Left = 12,
	Right = 13,
	UpLeft = 14,
	UpRight = 15,
	WantUp = 16,
	WantDown = 17,
	WantDownSlow = 18,
	WantUpSlow = 19,
	Sprinting = 20,
	AscendBlock = 21,
	DescendBlock = 22,
	SneakToggleDown = 23,
	PersistSneak = 24,
	StartSprinting = 25,
	StopSprinting = 26,
	StartSneaking = 27,
	StopSneaking = 28,
	StartSwimming = 29,
	StopSwimming = 30,
	StartJumping = 31,
	StartGliding = 32,
	StopGliding = 33,
	PerformItemInteraction = 34,
	PerformBlockActions = 35,
	PerformItemStackRequest = 36,
	HandledTeleport = 37,
	Emoting = 38,
	MissedSwing = 39,
	StartCrawling = 40,
	StopCrawling = 41,
	StartFlying = 42,
	StopFlying = 43,
	ClientAckServerData = 44,
	IsInClientPredictedVehicle = 45,
	PaddlingLeft = 46,
	PaddlingRight = 47,
	BlockBreakingDelayEnabled = 48,
	HorizontalCollision = 49,
	VerticalCollision = 50,
	DownLeft = 51,
	DownRight = 52,
	StartUsingItem = 53,
	IsCameraRelativeMovementEnabledDeprecated = 54,
	IsRotControlledByMoveDirectionDeprecated = 55,
	StartSpinAttack = 56,
	StopSpinAttack = 57,
	IsHotbarOnlyTouch = 58,
	JumpReleasedRaw = 59,
	JumpPressedRaw = 60,
	JumpCurrentRaw = 61,
	SneakReleasedRaw = 62,
	SneakPressedRaw = 63,
	SneakCurrentRaw = 64,
	InternalUpdate = 65,
	InputNum = 66,
};

class PlayerAuthInputPacket : public Packet {
public:
	static constexpr PacketID ID = PacketID::PlayerAuthInputPacket;

	Vec2<float> mRot;
	Vec3<float> mPos;
	float mYHeadRot;
	Vec3<float> mPosDelta;
	Vec2<float> mVehicleRot;
	Vec2<float> mAnalogMoveVector;
	Vec2<float> mMove;
	Vec2<float> mInteractRotation;
	Vec3<float> mCameraOrientation;
	Vec2<float> mRawMoveVector;
	std::bitset<66> mInputData;
	InputMode mInputMode;
	ClientPlayMode mPlayMode;
	NewInteractionModel mNewInteractionModel;
	uint64_t mClientTick;
	void* mItemUseTransaction;
	void* mItemStackRequest;
	std::array<std::byte, 24> mPlayerBlockActions;
	ActorUniqueID mClientPredictedVehicle;
	int32_t mSerializationMode;

	bool hasInputData(InputData input) const {
		return mInputData.test(static_cast<size_t>(input));
	}

	void setInputData(InputData input, bool value = true) {
		mInputData.set(static_cast<size_t>(input), value);
	}
};

static_assert(sizeof(PlayerAuthInputPacket) == 0xE0);

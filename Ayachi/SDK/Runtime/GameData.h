#pragma once
#include <cstdint>
namespace GameData {

	namespace Address {
		extern uintptr_t Platform_GameCore_winMain;

		extern uintptr_t KeyMap;
		extern uintptr_t GameCore_handleMouseInput;
		extern uintptr_t ClientInputUpdateSystemInternal_tickUpdateClientInput;
		extern uintptr_t KeyPressFunc;
		extern uintptr_t KeyMouseFunc;

		extern uintptr_t ClientInstance_grabCursor;
		extern uintptr_t ClientInstance_releaseCursor;
		extern uintptr_t ClientInstance_update;
		extern uintptr_t MinecraftGame_update;
		extern uintptr_t MinecraftGame_onDeviceLost;
		extern uintptr_t ClientInstanceScreenModel_sendChatMessage;
		extern uintptr_t ClientInstanceScreenModel_forwardSoundSubtitle;
		extern uintptr_t GuiData_displayClientMessage;
		extern uintptr_t AppPlatformGDK_releaseMouse;
		extern uintptr_t AppPlatform_GameCorePC_pickImage;
		extern uintptr_t MainWindow_windowProcCallback;

		extern uintptr_t Options_getGamma;
		extern uintptr_t Options_getPerspective;
		extern uintptr_t Options_setPerspective;
		extern uintptr_t Options_getHideHand;

		extern uintptr_t LevelRenderer_renderLevel;
		extern uintptr_t LevelRendererPlayer_getFov;
		extern uintptr_t LevelRendererPlayer_getFovForCameraReturn;
		extern uintptr_t LevelRendererPlayer_renderOutlineSelection;
		extern uintptr_t LevelRendererPlayer_renderHitSelect;
		extern uintptr_t ScreenView_setupAndRender;
		extern uintptr_t ItemInHandRenderer_renderFirstPerson;
		extern uintptr_t ItemInHandRenderer_renderOffhandItem;
		extern uintptr_t RenderController_getOverlayColor;
		extern uintptr_t BaseActorRenderer_renderText;
		extern uintptr_t Tessellator_begin;
		extern uintptr_t Tessellator_vertex;
		extern uintptr_t Tessellator_color;
		extern uintptr_t MeshHelpers_renderMeshImmediately;
		extern uintptr_t BaseActorRenderContext_Constructor;
		extern uintptr_t ItemRenderer_renderGuiItemNew;
		extern uintptr_t ActorRenderDispatcher_render;
		extern uintptr_t RenderMaterialGroup_common;

		extern uintptr_t LevelVTable;
		extern uintptr_t MultiPlayerLevel_subTick;
		extern uintptr_t Dimension_getTimeOfDay;
		extern uintptr_t Dimension_tick;
		extern uintptr_t Dimension_getSkyColor;
		extern uintptr_t Actor_attack;
		extern uintptr_t Actor_getNameTag;
		extern uintptr_t Actor_setNameTag;
		extern uintptr_t LocalPlayer_applyTurnDelta;
		extern uintptr_t UpdatePlayerFromCameraSystemUtil_updatePlayer;
		extern uintptr_t GuiMessageVector_emplaceBack;

		extern uintptr_t ItemStack_ItemStackBlock;
		extern uintptr_t ItemStackVtable;
		extern uintptr_t ItemStackBase_destructor;
		extern uintptr_t ItemStackBase_getHoverName;
		extern uintptr_t ItemStackBase_getDamageValue;

		extern uintptr_t MinecraftPackets_createPacket;
		extern uintptr_t RakPeer_GetAveragePing;

		extern uintptr_t MouseDevice_instance;
		extern uintptr_t ThirdPersonNametag;
		extern uintptr_t GpuInfo;
		extern uintptr_t MolangVariable_findOrAddVariableIndex;
		extern uintptr_t MolangVariableMap_getOrAddMolangVariable;
		extern uintptr_t GameArguments_onUri;
	}

	namespace VTableIndex {
		namespace Inventory {
			inline constexpr int getItem = 0x07;
		}

		namespace Item {
			inline constexpr int getMaxUseDuration = 0x05;
			inline constexpr int canDestroySpecial = 0x21;
			inline constexpr int getMaxDamage = 0x24;
			inline constexpr int isGlint = 0x28;
			inline constexpr int getDestroySpeed = 0x58;
		}

		namespace Actor {
			inline constexpr int remove = 0x0C;
			inline constexpr int getFiringPos = 0x0D;
			inline constexpr int getInterpolatedBodyRot = 0x0E;
			inline constexpr int getInterpolatedHeadRot = 0x0F;
			inline constexpr int getInterpolatedBodyYaw = 0x10;
			inline constexpr int getYawSpeedInDegreesPerSecond = 0x11;
			inline constexpr int isFireImmune = 0x13;
			inline constexpr int teleportTo = 0x16;
			inline constexpr int lerpMotion = 0x17;
			inline constexpr int normalTick = 0x19;
			inline constexpr int baseTick = 0x1A;
			inline constexpr int passengerTick = 0x1B;
			inline constexpr int startRiding = 0x1C;
			inline constexpr int addPassenger = 0x1D;
			inline constexpr int getEntityLocNameString = 0x1F;
			inline constexpr int isInvisible = 0x20;
			inline constexpr int canShowNameTag = 0x21;
			inline constexpr int getShadowRadius = 0x23;
			inline constexpr int getHeadLookVector = 0x24;
			inline constexpr int canInteractWithOtherEntitiesInGame = 0x25;
			inline constexpr int isImmobile = 0x28;
			inline constexpr int isSilentObserver = 0x29;
			inline constexpr int isSleeping = 0x2A;
			inline constexpr int setSleeping = 0x2B;
			inline constexpr int setSneaking = 0x2C;
			inline constexpr int isBlocking = 0x2D;
			inline constexpr int isAlive = 0x2F;
			inline constexpr int isOnFire = 0x30;
			inline constexpr int isSurfaceMob = 0x31;
			inline constexpr int isTargetable = 0x32;
			inline constexpr int setTarget = 0x33;
			inline constexpr int isValidTarget = 0x34;
			inline constexpr int setSitting = 0x38;
			inline constexpr int setStanding = 0x3B;
			inline constexpr int canPowerJump = 0x3C;
			inline constexpr int isEnchanted = 0x3D;
			inline constexpr int playAmbientSound = 0x3E;
			inline constexpr int onLightningHit = 0x43;
			inline constexpr int feed = 0x44;
			inline constexpr int handleEntityEvent = 0x45;
			inline constexpr int despawn = 0x47;
			inline constexpr int setCarriedItem = 0x4D;
			inline constexpr int getCarriedItem = 0x4E;
			inline constexpr int setOffhandSlot = 0x4F;
			inline constexpr int canFreeze = 0x55;
			inline constexpr int canChangeDimensionsUsingPortal = 0x58;
			inline constexpr int canAddPassenger = 0x5F;
			inline constexpr int canPickupItem = 0x60;
			inline constexpr int startSwimming = 0x65;
			inline constexpr int stopSwimming = 0x66;
			inline constexpr int buildDebugInfo = 0x67;
			inline constexpr int canObstructSpawningAndBlockPlacement = 0x6D;
			inline constexpr int swing = 0x70;
			inline constexpr int getMapDecorationRotation = 0x73;
			inline constexpr int getPassengerYRotation = 0x74;
			inline constexpr int add = 0x75;
			inline constexpr int drop = 0x76;
			inline constexpr int kill = 0x7B;
			inline constexpr int shouldDropDeathLoot = 0x7D;
			inline constexpr int hasDiedBefore = 0x82;
		}

		namespace Mob {
			inline constexpr int setSprinting = 0x8D;
			inline constexpr int getItemUseDuration = 0x96;
			inline constexpr int getAllEquipment = 0xA2;
		}

		namespace Player {
			inline constexpr int displayClientMessage = 0xC8;
			inline constexpr int getXUID = 0xE9;
		}

		namespace LocalPlayer {
			inline constexpr int isAutoJumpEnabled = 0xBD;
			inline constexpr int respawn = 0xBE;
			inline constexpr int resetRot = 0xBF;
			inline constexpr int isInTrialMode = 0xC0;
			inline constexpr int openInventory = 0xC6;
			inline constexpr int isLoading = 0xD2;
			inline constexpr int stopLoading = 0xD4;
			inline constexpr int getUserId = 0xD9;
			inline constexpr int addExperience = 0xDA;
			inline constexpr int addLevels = 0xDB;
			inline constexpr int isTeacher = 0xE1;
			inline constexpr int sendNetworkPacket = 0xE6;
			inline constexpr int getMaxChunkBuildRadius = 0xED;
		}

		namespace GameMode {
			inline constexpr int startDestroyBlock = 0x01;
			inline constexpr int destroyBlock = 0x02;
			inline constexpr int continueDestroyBlock = 0x03;
			inline constexpr int stopDestroyBlock = 0x04;
			inline constexpr int startBuildBlock = 0x05;
			inline constexpr int buildBlock = 0x06;
			inline constexpr int continueBuildBlock = 0x07;
			inline constexpr int stopBuildBlock = 0x08;
			inline constexpr int tick = 0x09;
			inline constexpr int getPickRange = 0x0A;
			inline constexpr int useItem = 0x0B;
			inline constexpr int useItemAsAttack = 0x0C;
			inline constexpr int useItemOn = 0x0D;
			inline constexpr int interact = 0x0E;
			inline constexpr int attack = 0x0F;
			inline constexpr int releaseUsingItem = 0x10;
		}

		namespace BlockType {
			inline constexpr int getNextBlockPermutation = 0x01;
			inline constexpr int getOutline = 0x08;
			inline constexpr int getVisualShapeInWorld = 0x09;
			inline constexpr int getVisualShape = 0x0A;
			inline constexpr int getUIShape = 0x0B;
		}

		namespace BlockSource {
			inline constexpr int getBlock = 0x01;
			inline constexpr int getBlockLayer = 0x02;
			inline constexpr int getExtraBlock = 0x04;
			inline constexpr int getLiquidBlock = 0x05;
			inline constexpr int hasBlock = 0x06;
			inline constexpr int containsAnyLiquid = 0x07;
			inline constexpr int isInWall = 0x09;
			inline constexpr int isUnderWater = 0x0A;
			inline constexpr int getMinHeight = 0x23;
			inline constexpr int getMaxHeight = 0x24;
			inline constexpr int hasBorderBlock = 0x0D;
			inline constexpr int getBrightness = 0x18;
			inline constexpr int areChunksFullyLoaded = 0x37;
			inline constexpr int isSolidBlockingBlock = 0x35;
		}

		namespace Level {
			inline constexpr int fetchEntity = 0x3E;
			inline constexpr int getRuntimeEntity = 0x40;
			inline constexpr int getPlayerByName = 0x44;
			inline constexpr int getPlayerByUniqueId = 0x46;
			inline constexpr int getPlayerByXuid = 0x47;
			inline constexpr int getPlatformPlayer = 0x48;
			inline constexpr int getPlayerFromServerId = 0x49;
			inline constexpr int getRuntimePlayer = 0x4A;
			inline constexpr int getNumRemotePlayers = 0x4B;
			inline constexpr int getPrimaryLocalPlayer = 0x4C;
			inline constexpr int tick = 0x77;
			inline constexpr int getTime = 0x84;
			inline constexpr int setTime = 0x85;
			inline constexpr int getSeed = 0x86;
			inline constexpr int isClientSide = 0x13D;
			inline constexpr int getRuntimeActorList = 0x145;
			inline constexpr int getPacketSender = 0x148;
			inline constexpr int hasStartWithMapEnabled = 0x158;
			inline constexpr int isEditorWorld = 0x159;
			inline constexpr int getAllowAnonymousBlockDropsInEditorWorlds = 0x15A;
			inline constexpr int isHardcore = 0x15B;
			inline constexpr int getTearingDown = 0x15F;
		}

		namespace GameRenderer {
			inline constexpr int destructor = 0x00;
		}

		namespace LevelRenderer {
			inline constexpr int destructor = 0x00;
			inline constexpr int onBlockChanged = 0x04;
			inline constexpr int onChunkUnloaded = 0x17;
			inline constexpr int takePicture = 0x1B;
		}

		namespace LevelRendererPlayer {
			inline constexpr int destructor = 0x00;
			inline constexpr int getCameraType = 0x01;
			inline constexpr int addCameraListenerToRenderChunkCoordinator = 0x02;
			inline constexpr int onAppSuspended = 0x03;
			inline constexpr int onAppResumed = 0x04;
			inline constexpr int onDeviceLost = 0x05;
			inline constexpr int onLowMemory = 0x06;
			inline constexpr int initResources = 0x07;
			inline constexpr int frameUpdate = 0x08;
			inline constexpr int tickLevelRendererCamera = 0x09;
			inline constexpr int tickRain = 0x0A;
			inline constexpr int updateViewArea = 0x0B;
			inline constexpr int callRenderNameTags = 0x0C;
			inline constexpr int extractNameTags = 0x0D;
			inline constexpr int callRenderCracks = 0x0E;
			inline constexpr int extractCracks = 0x0F;
			inline constexpr int renderEntityEffects = 0x10;
			inline constexpr int renderBlockEntities = 0x11;
			inline constexpr int setViewArea = 0x12;
			inline constexpr int getForceFog = 0x13;
			inline constexpr int setupFog = 0x14;
			inline constexpr int getAmbientBrightness = 0x15;
			inline constexpr int recalculateRenderDistance = 0x16;
			inline constexpr int preRenderUpdate = 0x17;
			inline constexpr int render = 0x18;
			inline constexpr int postRenderUpdate = 0x19;
			inline constexpr int notifyGeoChangedForAffectedEntities = 0x1A;
			inline constexpr int updateLevelCullerType = 0x1B;
			inline constexpr int queueRenderEntities = 0x1C;
			inline constexpr int releaseRespectiveResources = 0x1D;
			inline constexpr int setupViewArea = 0x1E;
			inline constexpr int getFogDistanceSettingType = 0x1F;
			inline constexpr int getFogDensitySettingType = 0x20;
			inline constexpr int getUnderwaterVisionClarity = 0x21;
		}

		namespace UIRenderContext {
			inline constexpr int destructor = 0x00;
			inline constexpr int getLineLength = 0x01;
			inline constexpr int getTextAlpha = 0x02;
			inline constexpr int setTextAlpha = 0x03;
			inline constexpr int drawDebugText = 0x04;
			inline constexpr int drawText = 0x05;
			inline constexpr int flushText = 0x06;
			inline constexpr int drawImage = 0x07;
			inline constexpr int drawNineslice = 0x08;
			inline constexpr int flushImages = 0x09;
			inline constexpr int beginSharedMeshBatch = 0x0A;
			inline constexpr int endSharedMeshBatch = 0x0B;
			inline constexpr int reserveSharedMeshBatch = 0x0C;
			inline constexpr int getSharedMeshBatchVertexCount = 0x0D;
			inline constexpr int drawRectangle = 0x0E;
			inline constexpr int fillRectangle = 0x0F;
			inline constexpr int increaseStencilRef = 0x10;
			inline constexpr int decreaseStencilRef = 0x11;
			inline constexpr int resetStencilRef = 0x12;
			inline constexpr int fillRectangleStencil = 0x13;
			inline constexpr int enableScissorTest = 0x14;
			inline constexpr int disableScissorTest = 0x15;
			inline constexpr int setClippingRectangle = 0x16;
			inline constexpr int setFullClippingRectangle = 0x17;
			inline constexpr int saveCurrentClippingRectangle = 0x18;
			inline constexpr int restoreSavedClippingRectangle = 0x19;
			inline constexpr int getFullClippingRectangle = 0x1A;
			inline constexpr int updateCustom = 0x1B;
			inline constexpr int renderCustom = 0x1C;
			inline constexpr int cleanup = 0x1D;
			inline constexpr int removePersistentMeshes = 0x1E;
			inline constexpr int getTexture = 0x1F;
			inline constexpr int getZippedTexture = 0x20;
			inline constexpr int unloadTexture = 0x21;
			inline constexpr int getUITextureInfo = 0x22;
			inline constexpr int touchTexture = 0x23;
			inline constexpr int getMeasureStrategy = 0x24;
			inline constexpr int snapImageSizeToGrid = 0x25;
			inline constexpr int snapImagePositionToGrid = 0x26;
			inline constexpr int notifyImageEstimate = 0x27;
		}

		namespace ClientInstance {
			inline constexpr int tick = 0x16;
			inline constexpr int update = 0x18;
			inline constexpr int getRegion = 0x1E;
			inline constexpr int getLocalPlayer = 0x1F;
			inline constexpr int getMinecraftGame = 0x4F;
			inline constexpr int getMouseGrabbed = 0x92;
			inline constexpr int getLevel = 0xA7;
			inline constexpr int isInWorldAndNotShowingAnyMenuScreens = 0x9D;
			inline constexpr int getOptions = 0xAD;
			inline constexpr int getGameRenderer = 0xB9;
			inline constexpr int getLevelRenderer = 0xBA;
			inline constexpr int getGuiData = 0xD8;
			inline constexpr int getPacketSender = 0x124;
			inline constexpr int getInput = 0x12E;
			inline constexpr int grabMouse = 0x135;
			inline constexpr int releaseMouse = 0x136;
			inline constexpr int getItemRenderer = 0x161;
		}

		namespace ItemRenderer {
			inline constexpr int render = 0x02;
		}

		namespace Packet {
			inline constexpr int getId = 0x01;
			inline constexpr int getName = 0x02;
		}

		namespace PacketSender {
			inline constexpr int isInitialized = 0x01;
			inline constexpr int send = 0x02;
			inline constexpr int sendToServer = 0x04;
		}

		namespace PacketHandlerDispatcher {
			inline constexpr int handle = 0x01;
		}

		namespace DirectX {
			namespace D3D11 {
				inline constexpr int present = 8;
				inline constexpr int resizeBuffers = 13;
				inline constexpr int resizeBuffers1 = 39;
			}

			namespace D3D12 {
				inline constexpr int present = 140;
				inline constexpr int resizeBuffers = 145;
				inline constexpr int resizeBuffers1 = 171;
			}
		}
	}

	void init();
}

namespace Addresses = GameData::Address;

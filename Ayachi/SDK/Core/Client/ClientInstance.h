#pragma once
#include <string>
#include "../../../Utils/Maths.h"
#include "../../../Utils/MemoryUtil.h"
#include "../../Network/PacketSender.h"
#include "../../Render/GameRenderer.h"
#include "../../Render/ItemInHandRenderer.h"
#include "../../Render/ItemRenderer.h"
#include "../../Render/LevelRenderer.h"
#include "../../World/Actor/Actor.h"
#include "../../World/Actor/Player/LocalPlayer.h"
#include "../../World/Level/BlockSource.h"
#include "../../World/Level/Level.h"
#include "../../World/Level/Minecraft.h"
#include "MinecraftGame.h"

#include "../GUI/GuiData.h"

class ClientInstance {
public:
	CLASS_MEMBER(MinecraftGame*, minecraftGame, 0x1A0);
	CLASS_MEMBER(Minecraft*, minecraft, 0x1A8);
	CLASS_MEMBER(LevelRenderer*, levelRenderer, 0x1B8);
	CLASS_MEMBER(PacketSender*, packetSender, 0x1C8);
	CLASS_MEMBER(void*, inputHandler, 0x1D8);
	CLASS_MEMBER(Vec2<float>, cursorPos, 0x580);

	static ClientInstance* get();
	static void reset();

	void tick();
	bool update(bool isInitFinished);
	BlockSource* getRegion();
	LocalPlayer* getLocalPlayer();
	Actor* getCameraActor();
	Actor* getCameraTargetActor();
	Minecraft* getServerData();

	bool isLeavingGame() const;
	bool isDestroyingGame() const;
	bool isShuttingDown() const;
	bool isSplitScreenActive() const;
	bool isExitingLevel() const;
	bool isInBedScreen() const;
	bool isInDeathScreen() const;
	bool isKeyboardEnabled() const;
	bool hasCommands() const;
	int getSplitScreenCount() const;
	bool isShowingLoadingScreen() const;
	bool isReadyToRender() const;

	bool useController() const;
	bool useTouchscreen() const;
	bool getMouseGrabbed() const;
	bool currentInputModeIsMouseAndKeyboard() const;
	bool allowPicking() const;
	bool isShowingMenu() const;
	bool isShowingPauseScreen() const;
	bool isShowingProgressScreen() const;
	bool isShowingWorldProgressScreen() const;
	bool isShowingRealmsProgressScreen() const;
	bool isShowingDeathScreen() const;
	bool isShowingServerForm() const;
	bool isScreenReplaceable() const;
	Level* getLevel();
	bool isWorldActive() const;
	bool isInRealm();
	bool readyForShutdown() const;
	bool isPrimaryClient() const;
	bool isEduMode() const;
	bool isGamepadCursorEnabled() const;
	bool isInControlCustomization() const;
	bool hasLevel() const;
	bool isPreGame() const;
	bool isInMultiplayerGame() const;
	bool isMultiPlayerClient() const;
	bool isInWorldAndNotShowingAnyMenuScreens() const;
	MinecraftGame* getMinecraftGame();
	void* getOptions();
	GameRenderer* getGameRenderer();
	LevelRenderer* getLevelRenderer();
	bool shouldRenderUICursor() const;
	bool getRenderPlayerModel() const;
	float getGuiScale() const;
	GuiData* getGuiData();
	PacketSender* getPacketSender();
	void* getInput();
	int getCurrentInputMode() const;
	void* getMinecraftInput() const;
	Vec3<float> getLastPointerLocation();
	Vec2<float> getTouchPointerLocation(int id) const;
	int getActionPointerId() const;
	int getControllerId() const;
	bool hasConnectedController() const;
	void refocusMouse(bool lostMouse);
	void* getSoundEngine() const;
	void* getMusicManager() const;
	bool isFullVanillaPackOnStack() const;
	ItemInHandRenderer* getItemInHandRenderer();
	ItemRenderer* getItemRenderer();

	void grabMouse();
	void releaseMouse();
	void grabCursor();
	void releaseCursor();

private:
	static ClientInstance* instance;
};

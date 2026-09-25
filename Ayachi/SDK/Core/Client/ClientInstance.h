#pragma once
#include <string>
#include "../../../Utils/Maths.h"
#include "../../../Utils/MemoryUtil.h"
#include "../../World/Level/BlockSource.h"

class Minecraft;
class MinecraftGame;
class LevelRenderer;
class PacketSender;
class ClientInputHandler;
class LocalPlayer;
class Level;
class IOptionRegistry;
class GameRenderer;
class ItemRenderer;

#include "../GUI/GuiData.h"

class ClientInstance {
public:
	CLASS_MEMBER(MinecraftGame*, minecraftGame, 0x1A0);
	CLASS_MEMBER(Minecraft*, minecraft, 0x1A8);
	CLASS_MEMBER(LevelRenderer*, levelRenderer, 0x1B8);
	CLASS_MEMBER(PacketSender*, packetSender, 0x1C8);
	CLASS_MEMBER(ClientInputHandler*, inputHandler, 0x1D8);
	CLASS_MEMBER(Vec2<float>, cursorPos, 0x580);

	static ClientInstance* get();

	bool update(bool isInitFinished);
	BlockSource* getRegion();
	LocalPlayer* getLocalPlayer();
	bool getMouseGrabbed();
	Level* getLevel();
	bool isInWorldAndNotShowingAnyMenuScreens() const;
	MinecraftGame* getMinecraftGame();
	IOptionRegistry* getOptions();
	GameRenderer* getGameRenderer();
	LevelRenderer* getLevelRenderer();
	GuiData* getGuiData();
	PacketSender* getPacketSender();
	ClientInputHandler* getInput();
	ItemRenderer* getItemRenderer();

	void grabCursor();
	void releaseCursor();

private:
	static ClientInstance* instance;
};

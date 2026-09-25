#include "TestModule.h"

#include "../../../../../../SDK/Runtime/GameData.h"
#include "../../../../../../SDK/World/Actor/Player/LocalPlayer.h"
#include "../../../../../../SDK/World/Inventory/Inventory.h"
#include "../../../../../../SDK/World/Inventory/PlayerInventory.h"
#include "../../../../../../SDK/World/Item/Item.h"
#include "../../../../../../SDK/World/Item/ItemStack.h"

TestModule::TestModule()
	: Module("TestModule", "Validate the Item SDK using the selected hotbar item", Category::MISC) {}

std::string TestModule::getModeText() {
	switch (state) {
	case TestState::Passed:
		return "PASS";
	case TestState::Failed:
		return "FAIL";
	default:
		return "WAIT";
	}
}

void TestModule::onEnable() {
	state = TestState::Waiting;
	tickCounter = 9;
	completed = false;
	emptyReported = false;
}

void TestModule::onTick(Level*) {
	if (completed)
		return;

	if (++tickCounter < 10)
		return;

	tickCounter = 0;
	runTest();
}

void TestModule::fail(const char* reason) {
	state = TestState::Failed;
	completed = true;
	Game::DisplayClientMessage("[ItemSDKTest] FAIL: %s", reason);
}

void TestModule::runTest() {
	LocalPlayer* player = Game::getLocalPlayer();
	if (player == nullptr)
		return;

	PlayerInventory* supplies = player->supplies;
	if (supplies == nullptr)
		return;

	Inventory* inventory = supplies->getInventory();
	if (inventory == nullptr)
		return;

	const int slot = supplies->getSelectedSlot();
	if (slot < 0 || slot > 8) {
		fail("selected hotbar slot is outside 0..8");
		return;
	}

	ItemStack* stack = inventory->getItem(slot);
	if (stack == nullptr) {
		fail("Inventory::getItem returned null");
		return;
	}
	Game::DisplayClientMessage("[ItemSDKTest] stack access OK: slot=%d ptr=%p", slot, stack);

	if (GameData::Address::ItemStackBase_getHoverName == 0) {
		fail("ItemStackBase::getHoverName signature is unresolved");
		return;
	}

	Item* item = stack->getItem();
	if (item == nullptr) {
		if (!emptyReported) {
			Game::DisplayClientMessage("[ItemSDKTest] WAIT: slot %d is empty", slot);
			emptyReported = true;
		}
		return;
	}

	const uint8_t count = stack->getCount();
	const int16_t id = item->getId();
	Game::DisplayClientMessage(
		"[ItemSDKTest] item access OK: ptr=%p id=%d count=%u",
		item, static_cast<int>(id), static_cast<unsigned int>(count));
	const std::string hoverName = stack->getHoverName();
	if (!stack->isValid() || count == 0 || hoverName.empty()) {
		fail("basic ItemStack fields or hover name are invalid");
		return;
	}

	state = TestState::Passed;
	completed = true;
	Game::DisplayClientMessage(
		"[ItemSDKTest] PASS: slot=%d hover=%s id=%d count=%u",
		slot, hoverName.c_str(), static_cast<int>(id), static_cast<unsigned int>(count));
}

#pragma once

#include <cstdint>

enum class InventoryTransactionType : uint32_t {
	Normal = 0,
	InventoryMismatch = 1,
	ItemUse = 2,
	ItemUseOnActor = 3,
	ItemRelease = 4,
};

enum class ItemStackRequestActionType : uint8_t {
	Take = 0,
	Place = 1,
	Swap = 2,
	Drop = 3,
	Destroy = 4,
	Consume = 5,
	Create = 6,
	PlaceInItemContainer = 7,
	TakeFromItemContainer = 8,
	ScreenLabTableCombine = 9,
	ScreenBeaconPayment = 10,
	ScreenHUDMineBlock = 11,
	CraftRecipe = 12,
	CraftRecipeAuto = 13,
	CraftCreative = 14,
	CraftRecipeOptional = 15,
	CraftRepairAndDisenchant = 16,
	CraftLoom = 17,
	CraftNonImplemented = 18,
	CraftResults = 19,
};

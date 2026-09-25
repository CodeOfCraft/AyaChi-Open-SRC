#pragma once
#include <vector>

#include "Actor.h"

class Mob : public Actor {
public:
	void setSprinting(bool sprinting) {
		MemoryUtil::CallVFunc<GameData::VTableIndex::Mob::setSprinting, void, bool>(this, sprinting);
	}

	int getItemUseDuration() {
		return MemoryUtil::CallVFunc<GameData::VTableIndex::Mob::getItemUseDuration, int>(this);
	}

	std::vector<const ItemStack*> getAllEquipment() const {
		std::vector<const ItemStack*> equipment;
		MemoryUtil::CallVFunc<
			GameData::VTableIndex::Mob::getAllEquipment,
			void,
			std::vector<const ItemStack*>&>(const_cast<Mob*>(this), equipment);
		return equipment;
	}
};

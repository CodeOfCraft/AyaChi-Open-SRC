#pragma once

#include <memory>
#include <string>

#include "EntityTraits.h"

class EntityRegistry : public std::enable_shared_from_this<EntityRegistry> {
public:
	std::string name;
	entt::basic_registry<EntityId> ownedRegistry;
	uint32_t id;
};

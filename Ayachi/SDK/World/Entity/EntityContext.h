#pragma once
#include "EntityRegistry.h"

class EntityContext {
public:
	EntityRegistry& registry;
	entt::basic_registry<EntityId>& enttRegistry;
	EntityId entity;
};

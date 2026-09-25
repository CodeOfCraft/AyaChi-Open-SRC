#pragma once
#include <utility>
#include "EntityRegistry.h"

class EntityContext {
public:
	EntityRegistry& registry;
	entt::basic_registry<EntityId>& enttRegistry;
	EntityId entity;

	[[nodiscard]] entt::basic_registry<EntityId>& getRegistry() noexcept { return enttRegistry; }
	[[nodiscard]] const entt::basic_registry<EntityId>& getRegistry() const noexcept { return enttRegistry; }
	[[nodiscard]] EntityId getEntityId() const noexcept { return entity; }

	template<typename T>
	[[nodiscard]] T* tryGetComponent() {
		return enttRegistry.try_get<T>(entity);
	}

	template<typename T>
	[[nodiscard]] const T* tryGetComponent() const {
		return std::as_const(enttRegistry).try_get<T>(entity);
	}

	template<typename T>
	[[nodiscard]] bool hasComponent() const {
		return std::as_const(enttRegistry).all_of<T>(entity);
	}

	template<typename T>
	bool removeComponent() {
		return enttRegistry.remove<T>(entity) != 0;
	}

	template<typename T, typename... Args>
	T& getOrAddComponent(Args&&... args) {
		return enttRegistry.get_or_emplace<T>(entity, std::forward<Args>(args)...);
	}
};

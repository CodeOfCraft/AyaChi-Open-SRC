#pragma once

#include <concepts>
#include <type_traits>

#include "../../../Libs/entt/entt.hpp"
#include "EntityId.h"
#include "EntityIdTraits.h"
#include "IEntityComponent.h"

template<>
struct entt::entt_traits<EntityId> : entt::basic_entt_traits<EntityIdTraits> {
	static constexpr std::size_t page_size = 2048;
};

template<typename Type>
	requires std::derived_from<Type, IEntityComponent>
struct entt::component_traits<Type> {
	using type = Type;
	static constexpr bool in_place_delete = true;
	static constexpr std::size_t page_size = 128 * !std::is_empty_v<Type>;
};

template<typename Type>
struct entt::storage_type<Type, EntityId> {
	using type = entt::basic_storage<Type, EntityId>;
};

template<typename Type>
	requires std::derived_from<Type, IEntityComponent>
struct entt::type_hash<Type> {
	[[nodiscard]] static constexpr id_type value() noexcept {
		return Type::type_hash;
	}
};

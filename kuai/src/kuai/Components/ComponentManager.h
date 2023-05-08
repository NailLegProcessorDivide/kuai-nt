#pragma once

#include "kpch.h"

#include "EntityManager.h"

// @cond
namespace kuai {
	using ComponentType = uint8_t;

	class ComponentManager;
	class IComponentContainer
	{
	public:
		virtual ~IComponentContainer() = default;

		virtual void onEntityDestroyed(EntityID entity) = 0;
	};

	template<typename T>
	class ComponentContainer : public IComponentContainer
	{
	public:
		template <typename ...Args>
		void insert(EntityID entity, Box<T> component)
		{
			KU_CORE_ASSERT(entityToIndex.find(entity) == entityToIndex.end(), "Added duplicate component to entity");

			entityToIndex[entity] = size;
			indexToEntity[size] = entity;

			components[size] = std::move(component);
	
			size++;
		}

		void remove(EntityID entity)
		{
			KU_CORE_ASSERT(entityToIndex.find(entity) != entityToIndex.end(), "Removing component that does not exist");

			// Update mappings s.t. entity of last component in array points to removed index and vice-versa
			size_t removeIndex = entityToIndex[entity];
			size_t lastElementIndex = size;

			EntityID lastElementEntity = indexToEntity[lastElementIndex];
			entityToIndex[lastElementEntity] = removeIndex;
			indexToEntity[removeIndex] = lastElementEntity;

			// Move last element of array to removed index so components are mapped properly
			components[removeIndex] = std::move(components[lastElementIndex]);

			// Erase mappings of removed component and index of last component in the array
			// The components array will remain tightly packed
			entityToIndex.erase(entity);
			indexToEntity.erase(lastElementIndex);

			lastElementIndex--;
		}

		T& get(EntityID entity)
		{
			KU_CORE_ASSERT(entityToIndex.find(entity) != entityToIndex.end(), "Retrieving component that does not exist");

			return *(components[entityToIndex[entity]].get()); // Return reference to entity's component
		}

		bool has(EntityID entity) const
		{
			return entityToIndex.find(entity) != entityToIndex.end();
		}

		EntityID getEntityIDFromComponent(const T& component) const
		{
			for (size_t index = 0; index < size; ++index)
			{
				if (components[index].get() == &component)
				{
					return indexToEntity.at(index);
				}
			}
			return -1;
		}

		virtual void onEntityDestroyed(EntityID entity) override
		{
			// If entity owns this component type
			if (entityToIndex.find(entity) != entityToIndex.end())
			{
				remove(entity);
			}
		}

		typename std::vector<T>::iterator begin() { return components.begin(); }
		typename std::vector<T>::iterator end() { return components.end(); }

	private:
		Box<T> components[MAX_ENTITIES];

		// Mappings from entities to their respective index in components array and vice-versa
		std::unordered_map<EntityID, size_t> entityToIndex;
		std::unordered_map<size_t, EntityID> indexToEntity;

		size_t size = 0;
	};

	class ComponentManager
	{
	public:
		template<typename T>
		void registerComponent()
		{
			const char* typeName = typeid(T).name();

			KU_CORE_ASSERT(componentTypes.find(typeName) == componentTypes.end(), "Registering a component type more than once")

			componentTypes.emplace(typeName, nextComponentType++);	// Increment for next component
			componentContainers.emplace(typeName, MakeRc<ComponentContainer<T>>());
		}

		template<typename T, typename... Args>
		void addComponent(EntityID entity, Args&&... args)
		{
			auto component = MakeBox<T>(args...);
			component->cm = this;
			component->id = entity;
			getComponentContainer<T>()->insert(entity, std::move(component));
		}

		template<typename T>
		void removeComponent(EntityID entity)
		{
			getComponentContainer<T>()->remove(entity);
		}
		
		template<typename T>
		T& getComponent(EntityID entity)
		{
			return getComponentContainer<T>()->get(entity);
		}

		template<typename T>
		bool hasComponent(EntityID entity)
		{
			return getComponentContainer<T>()->has(entity);
		}

		template<typename T>
		ComponentType getComponentType()
		{
			return componentTypes[typeid(T).name()];
		}

		void onEntityDestroyed(EntityID entity)
		{
			for (auto const& pair : componentContainers)
			{
				auto const& component = pair.second;
				component->onEntityDestroyed(entity);
			}
		}
		
	private:
		// Map of component names to their types (uint_8)
		std::unordered_map<const char*, ComponentType> componentTypes;
		// Map of component names to their containers
		std::unordered_map<const char*, Rc<IComponentContainer>> componentContainers;

		// Component type to be assigned to next registered component
		ComponentType nextComponentType = 0;

		template<typename T>
		Rc<ComponentContainer<T>> getComponentContainer()
		{
			const char* typeName = typeid(T).name();

			KU_CORE_ASSERT(componentTypes.find(typeName) != componentTypes.end(), "Component not registered");

			return std::static_pointer_cast<ComponentContainer<T>>(componentContainers[typeName]);
		}
	};
}
// @endcond

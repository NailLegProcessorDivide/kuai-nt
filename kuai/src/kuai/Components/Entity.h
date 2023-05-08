#pragma once

#include "EntityComponentSystem.h"
#include "Components.h"

// The entity class is a wrapper for an EntityID; so one can do entity.AddComponent<T>() instead of ECS.addComponent(entity, T)

namespace kuai {
	/** \class Entity
	*	\brief Base class for all game objects.
	*/
	class Entity
	{
	public:
		/// @private
		Entity(EntityComponentSystem* ECS) : ECS(ECS)
		{
			id = ECS->createEntity();
			addComponent<Transform>(); // Every entity has a transform
		}
		
		/// @private
		Entity(EntityComponentSystem* ECS, EntityID id) : ECS(ECS), id(id) // Wrap entity object over existing ID
		{
		}

		/**
		* Add a component T that can be constructed with ...args to this entity.
		*/
		template<class T, typename ...Args>
		T& addComponent(Args&& ...args)
		{				
			// Send a reference of this entity to the component
			ECS->template addComponent<T>(id, std::forward<Args>(args)...);
			return ECS->template getComponent<T>(id);
		}

		/**
		* Return component T if it is attached to this entity.
		*/
		template<class T>
		T& getComponent()
		{	
			return ECS->template getComponent<T>(id);
		}

		/**
		* Check if component T is attached to this entity.
		*/
		template<class T>
		bool hasComponent()
		{
			return ECS->template hasComponent<T>(id);
		}

		/**
		* Removes component T from this entity.
		*/
		template<class T>
		void removeComponent()
		{
			ECS->template removeComponent<T>(id);
		}

		/**
		* Every entity has a transform component; returns said component.
		*/
		Transform& getTransform()
		{
			return ECS->template getComponent<Transform>(id);
		}

		/// @private
		EntityID getId() const
		{
			return id;
		}

	private:
		EntityID id;
		EntityComponentSystem* ECS;
	};

}
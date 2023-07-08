#pragma once

namespace kuai {
	// Forward declarations
	class EntityComponentSystem; 
	class Entity;

	using EntityID = uint32_t;
	/**
	* Defines the game logic for a set of entities
	*/
	class System
	{
	public:
		virtual void update(float dt) = 0;

		virtual void insertEntity(EntityID id);

		virtual void removeEntity(EntityID id);

		void acceptSubset(bool val);

		std::vector<Entity>& getEntities();

	protected:
		EntityComponentSystem* ECS;

		std::vector<Entity> entities;

	private:
		bool hasEntity(EntityID id);

	private:
		bool acceptsSubset; // Add entities that have components which are a subset of the system's component mask

		friend class SystemManager;
		friend class EntityComponentSystem;
	};
}

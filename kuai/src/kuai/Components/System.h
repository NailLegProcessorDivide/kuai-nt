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
		System(bool acceptsSubset = false);

		virtual void update(float dt) = 0;

		virtual void insertEntity(EntityID entity);

		virtual void removeEntity(EntityID entity);

		std::vector<std::shared_ptr<Entity>>& getEntities();

	protected:
		std::vector<std::shared_ptr<Entity>> entities;
	private:
		bool hasEntity(EntityID entity);

		void setECS(EntityComponentSystem* ECS);

	private:
		EntityComponentSystem* ECS;

		bool acceptsSubset; // Add entities that have components which are a subset of the system's component mask

		friend class SystemManager;
	};
}

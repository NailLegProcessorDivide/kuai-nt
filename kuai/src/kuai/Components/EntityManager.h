#pragma once

// @cond
namespace kuai {

	const u32 MAX_ENTITIES = 10000;
	const u32 MAX_COMPONENTS = 32;

	using EntityID = u32;
	using ComponentMask = u32;

	/**
	* Manages creation and deletion of all entities (game objects)
	* Entities are stored as a unique integer ID
	* Component masks are bitsets that show what components an entity has
	*/
	class EntityManager
	{
	public:
		EntityManager()
		{
			for (size_t i = 0; i < MAX_ENTITIES; i++)
			{
				availableEntities.push_back(MAX_ENTITIES - i);
			}
		}

		EntityID createEntity()
		{
			KU_CORE_ASSERT(entityNo < MAX_ENTITIES, "Exceeded maximum number of entities");

			EntityID id = availableEntities.back();
			availableEntities.pop_back();
			componentMasks[id] = 0;
			entityNo++;

			return id;
		}

		void destroyEntity(EntityID id)
		{
			componentMasks[id] = 0;

			availableEntities.push_back(id);
			entityNo--;
		}

		ComponentMask getComponentMask(EntityID id)
		{
			return componentMasks[id];
		}

		void setComponentMask(EntityID id, ComponentMask componentMask)
		{
			componentMasks[id] = componentMask;
		}

	private:
		// List of unused entity IDs
		std::vector<EntityID> availableEntities;
		// Number of entities currently in use
		u32 entityNo = 0;

		// Components associated with each entity
		ComponentMask componentMasks[MAX_ENTITIES];
	};
}
// @endcond
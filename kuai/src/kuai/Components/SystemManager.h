#pragma once

#include "EntityManager.h"
#include "System.h"

// @cond
namespace kuai {
	/**
	* Manages creation and deletion of all systems
	* Updates each system when an entity's component mask changes
	*/
	class SystemManager
	{
	public:
		template<typename T>
		Rc<T> registerSystem()
		{
			const char* typeName = typeid(T).name();

			KU_CORE_ASSERT(systems.find(typeName) == systems.end(), "Registering a system more than once");

			auto system = makeRc<T>();
			systems.insert({ typeName, system });
			return system;
		}

		template<typename T>
		void setComponentMask(ComponentMask componentMask)
		{
			const char* typeName = typeid(T).name();

			KU_CORE_ASSERT(systems.find(typeName) != systems.end(), "System not registered");

			systemMasks.insert({ typeName, componentMask });
		}

		void onEntityDestroyed(EntityID id)
		{
			for (auto const& pair : systems)
			{
				auto const& system = pair.second;
				system->removeEntity(id);
			}
		}

		void onEntityComponentMaskChanged(EntityID id, ComponentMask entityComponentMask)
		{
			// Update each system
			for (auto const& pair : systems)
			{
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& systemComponentMask = systemMasks[type];

				// If entity's component mask matches this system, add it to the system's list
				if (entityComponentMask == systemComponentMask || (system->acceptsSubset && (entityComponentMask & systemComponentMask)))
				{
					if (!system->hasEntity(id))
						system->insertEntity(id);
				}
				else
				{
					if (system->hasEntity(id))
						system->removeEntity(id);
				}
			}
		}

	private:
		// Maps each system type name to its system mask (what components does it update)
		std::unordered_map<const char*, ComponentMask> systemMasks;
		// Maps system type names to systems
		std::unordered_map<const char*, std::shared_ptr<System>> systems;
	};
}
// @endcond
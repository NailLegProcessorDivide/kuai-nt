#include "kpch.h"
#include "System.h"

#include "Entity.h"

namespace kuai {

	System::System(bool acceptsSubset) : acceptsSubset(acceptsSubset) {}

	void System::insertEntity(EntityID entity)
	{
		entities.push_back(MakeRc<Entity>(ECS, entity));
	}

	void System::removeEntity(EntityID entity)
	{
		for (size_t i = 0; i < entities.size(); i++)
		{
			if (entities[i]->getId() == entity)
			{
				entities.erase(entities.begin() + i);
				break;
			}
		}
	}

	std::vector<std::shared_ptr<Entity>>& System::getEntities()
	{
		return entities;
	}

	bool System::hasEntity(EntityID entity)
	{
		for (size_t i = 0; i < entities.size(); i++)
		{
			if (entities[i]->getId() == entity)
			{
				return true;
			}
		}
		return false;
	}

	void System::setECS(EntityComponentSystem* ECS)
	{
		this->ECS = ECS;
	}
}